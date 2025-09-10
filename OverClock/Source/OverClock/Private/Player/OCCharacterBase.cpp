#include "Player/OCCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/OCPlayerController.h"
#include "EnhancedInputComponent.h"

AOCCharacterBase::AOCCharacterBase()
	:WalkSpeed(300.0f),
	RunSpeed(600.0f),
	AirControlSpeed(0.2f),
	CrouchSpeed(100.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->AirControl = 0.2f * AirControlSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	//GetCharacterMovement()->bCrouch = true;
}

void AOCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
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
			if(PlayerController->CrunchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->CrunchAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::BeginCrouch
				);
				EnhancedInput->BindAction(
					PlayerController->CrunchAction,
					ETriggerEvent::Completed,
					this,
					&AOCCharacterBase::EndCrouch
				);
			}
			if(PlayerController->ShootAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ShootAction,
					ETriggerEvent::Triggered,
					this,
					&AOCCharacterBase::NormalAttack
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

void AOCCharacterBase::BeginCrouch()
{
	GetCharacterMovement()->Crouch();
}

void AOCCharacterBase::EndCrouch()
{
	GetCharacterMovement()->UnCrouch();
}

void AOCCharacterBase::NormalAttack()
{
	UE_LOG(LogTemp,Warning,TEXT("Player Shootting"))
}