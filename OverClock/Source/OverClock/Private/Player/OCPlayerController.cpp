#include "Player/OCPlayerController.h"
#include "Input/OCInputComponent.h"
#include "Data/DA_OCInputConfig.h"
#include "Data/OCGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Player/OCPlayerState.h"
#include "GA/GA_PeaceKeeper.h"
#include "GameplayTagContainer.h"
#include "Player/OCRevenant.h"

AOCPlayerController::AOCPlayerController()
{
	//bReplicates = true;
	InputComponent = CreateDefaultSubobject<UOCInputComponent>(TEXT("InputComponent"));
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
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Attack_Primary, ETriggerEvent::Triggered, this, &ThisClass::Input_Attack_Pressed);
	OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Reload, ETriggerEvent::Triggered, this, &ThisClass::Input_Reload);
	// 어빌리티 입력도 같은 방식으로 태그만 추가하면 됨 모르면 공부하셈
}

void AOCPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
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

void AOCPlayerController::Input_Attack_Pressed(const FInputActionValue& /*Value*/)
{
	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
	if (!PS) { UE_LOG(LogTemp, Warning, TEXT("[Input_Attack] PS NULL")); return; }

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.PeaceKeeper"));

	if (!ASC) {
		UE_LOG(LogTemp, Warning, TEXT("[Input_Attack] ASC NULL -> server fallback (tag)"));
		Server_EnsureAbilityGivenByTag(AttackTag);
		Server_TryActivateByTag(AttackTag);
		return;
	}

	// 상태 로그 그대로
	FGameplayTagContainer Owned; ASC->GetOwnedGameplayTags(Owned);
	UE_LOG(LogTemp, Log, TEXT("[Input_Attack] ASC OwnedTags: %s"), *Owned.ToStringSimple());
	const bool bHasFiring = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Firing"));
	const bool bHasCD = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Cooldown.PeaceKeeper"));
	UE_LOG(LogTemp, Log, TEXT("[Input_Attack] Has State.Firing=%d, Cooldown.PeaceKeeper=%d"), bHasFiring, bHasCD);

	// 태그 활성화 (기본)
	bool bActivated = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	UE_LOG(LogTemp, Warning, TEXT("[Attack] Activate %s => %s"), *AttackTag.ToString(), bActivated ? TEXT("SUCCESS") : TEXT("FAIL"));

	// 실패 시 서버가 태그로 지급/활성
	if (!bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input_Attack] Fallback to server give/activate (tag)"));
		Server_EnsureAbilityGivenByTag(AttackTag);
		Server_TryActivateByTag(AttackTag);
	}
}

// ─────────────── Reload (R) ───────────────
void AOCPlayerController::Input_Reload(const FInputActionValue& /*Value*/)
{
	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
	if (!PS) { UE_LOG(LogTemp, Warning, TEXT("[Input_RevReload] PS NULL")); return; }

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	const FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.RevReload"));

	if (!ASC) {
		UE_LOG(LogTemp, Warning, TEXT("[Input_RevReload] ASC NULL -> server fallback (tag)"));
		Server_EnsureAbilityGivenByTag(ReloadTag);
		Server_TryActivateByTag(ReloadTag);
		return;
	}

	// 상태 로그 그대로
	FGameplayTagContainer Owned; ASC->GetOwnedGameplayTags(Owned);
	UE_LOG(LogTemp, Log, TEXT("[Input_RevReload] ASC OwnedTags: %s"), *Owned.ToStringSimple());
	const bool bHasRevReloading = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.RevReloading"));
	const bool bHasCD = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Cooldown.RevReload"));
	UE_LOG(LogTemp, Log, TEXT("[Input_RevReload] Has State.RevReloading=%d, Cooldown.RevReload=%d"), bHasRevReloading, bHasCD);

	// 태그 활성화 (기본)
	bool bActivated = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
	UE_LOG(LogTemp, Warning, TEXT("[Input] Activate %s => %s"), *ReloadTag.ToString(), bActivated ? TEXT("SUCCESS") : TEXT("FAIL"));

	// 실패 시 서버가 태그로 지급/활성
	if (!bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input_RevReload] Fallback to server give/activate (tag)"));
		Server_EnsureAbilityGivenByTag(ReloadTag);
		Server_TryActivateByTag(ReloadTag);
	}
}



// ─────────────── Server RPC 구현 ───────────────

void AOCPlayerController::Server_EnsureAbilityGivenByTag_Implementation(FGameplayTag AbilityTag)
{
	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
	if (!PS) { UE_LOG(LogTemp, Warning, TEXT("[RPC] EnsureByTag: PS null")); return; }

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) { UE_LOG(LogTemp, Warning, TEXT("[RPC] EnsureByTag: ASC null")); return; }

	APawn* MyPawn = GetPawn();
	AOCRevenant* Rev = MyPawn ? Cast<AOCRevenant>(MyPawn) : nullptr; // 다른 캐릭터여도 동일 패턴으로 확장 가능
	if (!Rev) { UE_LOG(LogTemp, Warning, TEXT("[RPC] EnsureByTag: Pawn not Revenant")); return; }

	// 캐릭터의 맵에서 클래스 조회
	TSubclassOf<UGameplayAbility> AbilityClass = Rev->GetAbilityClassByTag(AbilityTag);
	if (!AbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RPC] EnsureByTag: No class mapped for tag %s"), *AbilityTag.ToString());
		return;
	}

	// 이미 있나?
	if (!ASC->FindAbilitySpecFromClass(AbilityClass))
	{
		ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, Rev));
		UE_LOG(LogTemp, Log, TEXT("[RPC] EnsureByTag: Gave %s for tag %s"), *AbilityClass->GetName(), *AbilityTag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("[RPC] EnsureByTag: Already has %s"), *AbilityClass->GetName());
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
	UE_LOG(LogTemp, Log, TEXT("[RPC] TryActivateByTag %s => %s"), *AbilityTag.ToString(), bOk ? TEXT("SUCCESS") : TEXT("FAIL"));
}