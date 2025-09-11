#include "Player/OCCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/OCPlayerController.h"
#include "EnhancedInputComponent.h"

AOCCharacterBase::AOCCharacterBase()
	:WalkSpeed(600.0f),
	RunSpeed(900.0f),
	JumpVelocity(600.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true;
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.f;

	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// OCAttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AOCCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOCCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		for (const auto& AbilityClass: StartupAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
		}
	}
}

void AOCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOCCharacterBase::SetupPlayerInputComponent(UInputComponent* EnhancedInputComp)
{
	Super::SetupPlayerInputComponent(EnhancedInputComp);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(EnhancedInputComp))
	{
		if (AOCPlayerController* PlayerController = Cast<AOCPlayerController>(GetController()))
		{
			ensure(PlayerController->MoveAction);
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::Move
				);
			}
			if(PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::Look
				);
			}
			if(PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::StartJump
				);
			}
			if(PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::StartSprint
				);
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AOCCharacterBase::StopSprint
				);
			}
			if(PlayerController->GANormalAttackAction)
			{
				EnhancedInput->BindAction(
					PlayerController->GANormalAttackAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::GANormalAttack
				);
			}
		}
	}
}

void AOCCharacterBase::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
		UE_LOG(LogTemp, Warning, TEXT("Move %f"), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AOCCharacterBase::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AOCCharacterBase::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AOCCharacterBase::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AOCCharacterBase::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AOCCharacterBase::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOCCharacterBase::GANormalAttack()
{
	UE_LOG(LogTemp,Warning,TEXT("[OCCharacterBase] GAS Normal Attack"));
}
