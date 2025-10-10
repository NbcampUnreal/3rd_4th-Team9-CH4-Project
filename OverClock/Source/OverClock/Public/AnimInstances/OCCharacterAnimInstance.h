#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/OCBaseAnimInstance.h"
#include "OCCharacterAnimInstance.generated.h"

class AOCCharacterBase_V2;
class UCharacterMovementComponent;

UCLASS()
class OVERCLOCK_API UOCCharacterAnimInstance : public UOCBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Owner")
	TObjectPtr<AOCCharacterBase_V2> OwningCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Owner")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
	float GroundSpeed = 0.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
	bool bHasAcceleration = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
	bool bIsFalling = false;
};
