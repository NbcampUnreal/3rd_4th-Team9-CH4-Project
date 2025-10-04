#include "Player/OCPlayerController.h"
#include "Input/OCInputComponent.h"
#include "Data/DA_OCInputConfig.h"
#include "Data/OCGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"
#include "AbilitySystemComponent.h"


AOCPlayerController::AOCPlayerController()
{
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

	OCInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_Ability_Pressed, &ThisClass::Input_Ability_Pressed);
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

void AOCPlayerController::Input_Ability_Pressed(FGameplayTag InInputTag)
{
	if (!InInputTag.IsValid()) return;

	if (UAbilitySystemComponent* ASC = GetOCASC())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(InInputTag);
		ASC->TryActivateAbilitiesByTag(TagContainer);
	}
	Server_TryActivateByTag(InInputTag);
}

void AOCPlayerController::Input_Ability_Released(FGameplayTag InInputTag)
{
	if (!InInputTag.IsValid()) return;
	Server_InputReleased_ByTag(InInputTag);
}

void AOCPlayerController::Server_InputReleased_ByTag_Implementation(FGameplayTag InInputTag)
{
	
}

void AOCPlayerController::Server_TryActivateByTag_Implementation(FGameplayTag InInputTag)
{
	if (!InInputTag.IsValid()) return;

	if (UAbilitySystemComponent* ASC = GetOCASC())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(InInputTag);
		ASC->TryActivateAbilitiesByTag(TagContainer);
	}
}

UAbilitySystemComponent* AOCPlayerController::GetOCASC() const
{
	if (const AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}