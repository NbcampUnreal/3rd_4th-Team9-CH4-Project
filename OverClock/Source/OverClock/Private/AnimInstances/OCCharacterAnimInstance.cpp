#include "AnimInstances/OCCharacterAnimInstance.h"
#include "Player/OCCharacterBase_V2.h"
#include "GameFramework/CharacterMovementComponent.h"

void UOCCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCharacter = Cast<AOCCharacterBase_V2>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		MovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UOCCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!OwningCharacter || !MovementComponent) return;

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = MovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
	bIsFalling = MovementComponent->IsFalling();
}
