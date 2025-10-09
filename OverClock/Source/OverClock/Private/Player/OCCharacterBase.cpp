#include "Player/OCCharacterBase.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "EnhancedInputComponent.h"
#include "Player/OCPlayerState.h"
#include "Data/DA_OCInputConfig.h"
#include "Data/DA_OCHeroStartUpData.h"


AOCCharacterBase::AOCCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

UAbilitySystemComponent* AOCCharacterBase::GetAbilitySystemComponent() const
{
	if (UAbilitySystemComponent* ASC = GetASC())
	{
		return ASC;
	}
	return nullptr;
}

void AOCCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	ResolveData();
	if (!InputConfig) return;

	if (UInputAction* InputAction = InputConfig->FindNativeInputActionByTag(FGameplayTag::RequestGameplayTag(TEXT("Input.Move"))))
	{
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AOCCharacterBase::Input_Move);
	}
	if (UInputAction* InputAction = InputConfig->FindNativeInputActionByTag(FGameplayTag::RequestGameplayTag(TEXT("Input.Look"))))
	{
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AOCCharacterBase::Input_Look);
	}
	if (UInputAction* InputAction = InputConfig->FindNativeInputActionByTag(FGameplayTag::RequestGameplayTag(TEXT("Input.Jump"))))
	{
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AOCCharacterBase::Input_Jump_Pressed);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AOCCharacterBase::Input_Jump_Released);
	}

	AutoBindAbilityInputs(EnhancedInputComponent);
	
}

void AOCCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitASCFromPalyerState();

	if (HasAuthority())
	{
		ResolveData();
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			if (HeroStartUpData)
			{
				HeroStartUpData->GiveToAbilitySystemComponent(ASC, 1);
			}
		}
	}
}

void AOCCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitASCFromPalyerState();
}

void AOCCharacterBase::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.IsNearlyZero()) return;
	
	const FRotator MovementRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);

	if (!FMath::IsNearlyZero(MovementVector.X))
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.X);
	}

	if (!FMath::IsNearlyZero(MovementVector.Y))
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.Y);
	} 
}

void AOCCharacterBase::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(LookVector.X))
	{
		AddControllerYawInput(LookVector.X);
	}

	if (!FMath::IsNearlyZero(LookVector.Y))
	{
		AddControllerPitchInput(LookVector.Y);
	}
}

void AOCCharacterBase::Input_Jump_Pressed(const FInputActionValue& Value)
{
	Jump();
}

void AOCCharacterBase::Input_Jump_Released(const FInputActionValue& Value)
{
	StopJumping();
}

void AOCCharacterBase::Input_Ability_Pressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		if (IsLocallyControlled())
		{
			for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
				{
					ASC->AbilitySpecInputPressed(Spec);
					if (!Spec.IsActive())
					{
						ASC->TryActivateAbility(Spec.Handle);
					}
				}
			}
		}
		Server_Ability_Pressed(InInputTag);
	}
}

void AOCCharacterBase::Input_Ability_Released(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		if (IsLocallyControlled())
		{
			for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
				{
					ASC->AbilitySpecInputReleased(Spec);
				}
			}
		}
		Server_Ability_Released(InInputTag);
	}
}

void AOCCharacterBase::InitASCFromPalyerState()
{
	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
		ASCWeak = PS->GetAbilitySystemComponent();
	}
	else
	{
		ASCWeak.Reset();
	}
}

void AOCCharacterBase::ResolveData()
{
	if (!InputConfig && InputConfigAsset.IsValid())
	{
		InputConfig = InputConfigAsset.Get();
	}
	if (!InputConfig && InputConfigAsset.ToSoftObjectPath().IsValid())
	{
		InputConfig = Cast<UDA_OCInputConfig>(InputConfigAsset.LoadSynchronous());
	}

	if (!HeroStartUpData && HeroStartUpDataAsset.IsValid())
	{
		HeroStartUpData = HeroStartUpDataAsset.Get();
	}
	if (!HeroStartUpData && HeroStartUpDataAsset.ToSoftObjectPath().IsValid())
	{
		HeroStartUpData = Cast<UDA_OCHeroStartUpData>(HeroStartUpDataAsset.LoadSynchronous());
	}
}

void AOCCharacterBase::AutoBindAbilityInputs(UEnhancedInputComponent* EnhancedInputComponent)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC || !InputConfig) return;

	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(), AbilitySpecs, false);

	for (FGameplayAbilitySpec* Spec : AbilitySpecs)
	{
		if (!Spec) continue;

		const FGameplayTagContainer& SourceTags = Spec->GetDynamicSpecSourceTags();
		for (const FGameplayTag& Tag : SourceTags)
		{
			if (!Tag.IsValid()) continue;

			if (UInputAction* InputAction = InputConfig->FindAbilityInputActionByTag(Tag))
			{
				EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AOCCharacterBase::Input_Ability_Pressed, Tag);
				EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AOCCharacterBase::Input_Ability_Released, Tag);
			}
		}
	}
}

UAbilitySystemComponent* AOCCharacterBase::GetASC() const
{
	if (ASCWeak.IsValid())
	{
		return ASCWeak.Get();
	}

	if (const AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

void AOCCharacterBase::Server_Ability_Pressed_Implementation(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;
	if (UAbilitySystemComponent* ASC = GetASC())
	{
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
			{
				ASC->AbilitySpecInputPressed(Spec);
				if (!Spec.IsActive())
				{
					ASC->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void AOCCharacterBase::Server_Ability_Released_Implementation(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;
	if (UAbilitySystemComponent* ASC = GetASC())
	{
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
			{
				ASC->AbilitySpecInputReleased(Spec);
			}
		}
	}
}
