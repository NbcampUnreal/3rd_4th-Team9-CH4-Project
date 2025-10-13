#include "Player/OCPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Input/OCInputComponent.h"
#include "Data/DA_OCInputConfig.h"
#include "Data/OCGameplayTags.h"
#include "EnhancedInputSubsystems.h"

#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "Abilities/OCAbilityDataAsset.h"
#include "GameFramework/Character.h"
#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"
#include "GA/GA_RangedAttack.h"
#include "Player/OCRevenant.h"
#include "OverClock.h"

AOCPlayerController::AOCPlayerController()
{
	//bReplicates = true;
	InputComponent = CreateDefaultSubobject<UOCInputComponent>(TEXT("InputComponent"));
}

void AOCPlayerController::PostNetInit()
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::PostNetInit();

	OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

void AOCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"));
	
	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
			}
		}
	}
}

void AOCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UOCInputComponent* OCInputComponent = Cast<UOCInputComponent>(InputComponent);
	if (!ensureAlwaysMsgf(OCInputComponent, TEXT("InputComponent must be UOCInputComponent"))) return;
	if (!ensureAlways(InputConfigDataAsset)) return;

	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump_Pressed);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Jump, ETriggerEvent::Completed, this, &ThisClass::Input_Jump_Released);
	
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Attack_Alt, ETriggerEvent::Started, this, &ThisClass::Input_Attack_Alt);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Skill_Active, ETriggerEvent::Started, this, &ThisClass::Input_Skill_Active);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Ultimate, ETriggerEvent::Started, this, &ThisClass::Input_Ultimate);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Interact, ETriggerEvent::Started, this, &ThisClass::Input_Interact);

	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Attack_Primary, ETriggerEvent::Started, this, &ThisClass::Input_Attack_Pressed);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Reload, ETriggerEvent::Started, this, &ThisClass::Input_Reload);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Attack_Skill1, ETriggerEvent::Started, this, &ThisClass::Input_Skill1);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Attack_Skill2, ETriggerEvent::Started, this, &ThisClass::Input_Skill2);
	// 어빌리티 입력도 같은 방식으로 태그만 추가하면 됨 모르면 공부하셈
}

void AOCPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 캐릭터의 GA Tag를 AbilityStruct에 대입
	if (AOCCharacterBase* C = Cast<AOCCharacterBase>(InPawn))
	{
		if (AbilityDataAsset)
		{
			AbilityStruct = AbilityDataAsset->CharacterAbilityTags[C->GetCurrentTag()];

			return;
		}

		//확인용 Log
		UE_LOG(LogTemp, Warning, TEXT("Character Ability Skill_Active : %s"), *AbilityStruct.InputTag_Skill_Active.ToString());
	}

	// PS(Owner)에 ASC가 붙어있다는 가정 → Avatar를 현재 Pawn으로 초기화
	if (APlayerState* PS = GetPlayerState<APlayerState>())
	{
		 GetASC()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AOCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOCPlayerController,AbilityStruct);

}

UAbilitySystemComponent* AOCPlayerController::GetASC() const
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PlayerState))
	{
		return ASI->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AOCPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//if (APlayerState* PS = GetPlayerState<APlayerState>())
	// {
	// 	GetASC()->InitAbilityActorInfo(PS, GetPawn());
	// }
}

void AOCPlayerController::Input_Move(const FInputActionValue& Value)
{
	APawn* P = GetPawn();
	if (!P) return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.IsNearlyZero()) return;
	
	const FRotator MovementRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);

	if (!FMath::IsNearlyZero(MovementVector.X))
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		P->AddMovementInput(ForwardDirection, MovementVector.X);
	}

	if (!FMath::IsNearlyZero(MovementVector.Y))
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		P->AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void AOCPlayerController::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(LookVector.X))
	{
		AddYawInput(LookVector.X);
	}

	if (!FMath::IsNearlyZero(LookVector.Y))
	{
		AddPitchInput(LookVector.Y);
	}
}

void AOCPlayerController::Input_Jump_Pressed(const FInputActionValue& Value)
{
	ACharacter* C = GetCharacter();
	if (C)
	{
		C->Jump();
	}

	/*if (AOCCharacterBase* C = Cast<AOCCharacterBase>(GetPawn()))
	{
		C->Jump();
	}*/
}

void AOCPlayerController::Input_Jump_Released(const FInputActionValue& Value)
{
	ACharacter* C = GetCharacter();
	if (C)
	{
		C->StopJumping();
	}

	/*if (AOCCharacterBase* C = Cast<AOCCharacterBase>(GetPawn()))
	{
		C->StopJumping();
	}*/
}

void AOCPlayerController::Input_Attack_Alt()
{
	Server_ActivateSkill(AbilityStruct.InputTag_Attack_Alt);
}

void AOCPlayerController::Input_Skill_Active()
{
	Server_ActivateSkill(AbilityStruct.InputTag_Skill_Active);
}

void AOCPlayerController::Input_Ultimate()
{
	UE_LOG(LogTemp, Warning, TEXT("Input_Ultimate"))
	Server_ActivateSkill(AbilityStruct.InputTag_Ultimate);
}

void AOCPlayerController::Input_Interact()
{
	Server_ActivateSkill(AbilityStruct.InputTag_Interact);
}

// ─────────────── Ability Inputs (간결화) ───────────────

void AOCPlayerController::TriggerAbilityByTag(const FGameplayTag& AbilityTag)
{
	if (UAbilitySystemComponent* ASC = GetASC())
	{
		if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag)))
		{
			return;
		}
	}

	Server_EnsureAbilityGivenByTag(AbilityTag);
	Server_TryActivateByTag(AbilityTag);
}

void AOCPlayerController::Input_Attack_Pressed()
{
	TriggerAbilityByTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.RangedAttack")));
}

void AOCPlayerController::Input_Reload()
{
	TriggerAbilityByTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Reload")));
}

void AOCPlayerController::Input_Skill1()
{
	TriggerAbilityByTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag.Attack.Skill1")));
}

void AOCPlayerController::Input_Skill2()
{
	TriggerAbilityByTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag.Attack.Skill2")));
}
// ─────────────── Server RPC 구현 ───────────────

void AOCPlayerController::Server_ActivateSkill_Implementation(FGameplayTag AbilityTag)
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(AbilityTag);
			UE_LOG(LogTemp, Warning, TEXT("Tag : %s"), *AbilityTag.ToString())
			ASC->TryActivateAbilitiesByTag(TagContainer);
		}
	}
}

void AOCPlayerController::Input_Attack_Pressed(const FInputActionValue& /*Value*/)
{
// }
// AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
// if (!PS) { UE_LOG(LogTemp, Warning, TEXT("[Input_Attack] PS NULL")); return; }
//
// UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
// const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.PeaceKeeper"));
//
// if (!ASC) {
// 	UE_LOG(LogTemp, Warning, TEXT("[Input_Attack] ASC NULL -> server fallback (tag)"));
// 	Server_EnsureAbilityGivenByTag(AttackTag);
// 	Server_TryActivateByTag(AttackTag);
// 	return;
// }
//
// 	// 상태 로그 그대로
// 	FGameplayTagContainer Owned; ASC->GetOwnedGameplayTags(Owned);
// 	UE_LOG(LogTemp, Log, TEXT("[Input_Attack] ASC OwnedTags: %s"), *Owned.ToStringSimple());
// 	const bool bHasFiring = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Firing"));
// 	const bool bHasCD = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Cooldown.PeaceKeeper"));
// 	UE_LOG(LogTemp, Log, TEXT("[Input_Attack] Has State.Firing=%d, Cooldown.PeaceKeeper=%d"), bHasFiring, bHasCD);
//
// 	// 태그 활성화 (기본)
// 	bool bActivated = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
// 	UE_LOG(LogTemp, Warning, TEXT("[Attack] Activate %s => %s"), *AttackTag.ToString(), bActivated ? TEXT("SUCCESS") : TEXT("FAIL"));
//
// 	// 실패 시 서버가 태그로 지급/활성
// 	if (!bActivated)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("[Input_Attack] Fallback to server give/activate (tag)"));
// 		Server_EnsureAbilityGivenByTag(AttackTag);
// 		Server_TryActivateByTag(AttackTag);
// 	}
}

// ─────────────── Reload (R) ───────────────
void AOCPlayerController::Input_Reload(const FInputActionValue& /*Value*/)
{
// 	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
// 	if (!PS) { UE_LOG(LogTemp, Warning, TEXT("[Input_RevReload] PS NULL")); return; }
//
// 	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
// 	const FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.RevReload"));
//
// 	if (!ASC) {
// 		UE_LOG(LogTemp, Warning, TEXT("[Input_RevReload] ASC NULL -> server fallback (tag)"));
// 		Server_EnsureAbilityGivenByTag(ReloadTag);
// 		Server_TryActivateByTag(ReloadTag);
// 		return;
// }
//
// 	// 상태 로그 그대로
// 	FGameplayTagContainer Owned; ASC->GetOwnedGameplayTags(Owned);
// 	UE_LOG(LogTemp, Log, TEXT("[Input_RevReload] ASC OwnedTags: %s"), *Owned.ToStringSimple());
// 	const bool bHasRevReloading = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.RevReloading"));
// 	const bool bHasCD = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Cooldown.RevReload"));
// 	UE_LOG(LogTemp, Log, TEXT("[Input_RevReload] Has State.RevReloading=%d, Cooldown.RevReload=%d"), bHasRevReloading, bHasCD);
//
// 	// 태그 활성화 (기본)
// 	bool bActivated = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
// 	UE_LOG(LogTemp, Warning, TEXT("[Input] Activate %s => %s"), *ReloadTag.ToString(), bActivated ? TEXT("SUCCESS") : TEXT("FAIL"));
//
// 	// 실패 시 서버가 태그로 지급/활성
// 	if (!bActivated)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("[Input_RevReload] Fallback to server give/activate (tag)"));
// 		Server_EnsureAbilityGivenByTag(ReloadTag);
// 		Server_TryActivateByTag(ReloadTag);
// 	}
}
//
//
//
// // ─────────────── Server RPC 구현 ───────────────
//
void AOCPlayerController::Server_EnsureAbilityGivenByTag_Implementation(FGameplayTag AbilityTag)
{
	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

	AOCCharacterBase* Char = Cast<AOCCharacterBase>(GetPawn());

	TSubclassOf<UGameplayAbility> AbilityClass = Char->GetAbilityClassByTag(AbilityTag);
	if (!AbilityClass) return;

	// 이미 있지 않으면 부여
	if (!ASC->FindAbilitySpecFromClass(AbilityClass))
	{
		ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, /*Level*/1, /*InputID*/0, Char));
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Log, TEXT("[RPC] EnsureByTag: Gave %s for tag %s"),
			*AbilityClass->GetName(), *AbilityTag.ToString());
#endif
	}
}

void AOCPlayerController::Server_TryActivateByTag_Implementation(FGameplayTag AbilityTag)
{
	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
	if (!PS) return;
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer TagContainer;

	TagContainer.AddTag(AbilityTag);

	const bool bOk = ASC->TryActivateAbilitiesByTag(TagContainer);
}