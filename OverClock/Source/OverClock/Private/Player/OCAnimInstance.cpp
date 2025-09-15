#include "Player/OCAnimInstance.h"

#include "Player/OCCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UOCAnimInstance::UOCAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
}

void UOCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter || !OwnerCharacterMovementComponent) return;

	//멤버변수 가져오기
}


