#include "Player/OCAnimInstance.h"

#include "Player/OCCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UOCAnimInstance::UOCAnimInstance(const FObjectInitializer& ObjectInitializer)
	:
	Super(ObjectInitializer),
	bShouldMove(0),
	Velocity(FVector::ZeroVector),
	GroundSpeed(0.f),
	bIsFalling(0),
	bIsAccelerating(0),
	AimRotation(0.f)
{
}

void UOCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<AOCCharacterBase>(GetOwningActor());
	if (OwnerCharacter)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("AnimInitiolize Fail"));
	}
}

void UOCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter && OwnerCharacterMovementComponent)
	{
		bShouldMove = GroundSpeed > 0.f;
		Velocity = OwnerCharacter->GetVelocity();
		GroundSpeed = OwnerCharacter->GetVelocity().Size();
		bIsFalling = OwnerCharacterMovementComponent->IsFalling();
		bIsAccelerating = OwnerCharacterMovementComponent->GetCurrentAcceleration().Size() > 0.f;
		AimRotation = OwnerCharacter->GetAimRotation();
	}
}


