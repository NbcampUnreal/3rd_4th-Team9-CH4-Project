#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OCAnimInstance.generated.h"

class AOCCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class OVERCLOCK_API UOCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UOCAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY()
	TObjectPtr<AOCCharacterBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;
};
