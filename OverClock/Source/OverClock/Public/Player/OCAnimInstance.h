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
	UOCAnimInstance(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TObjectPtr<AOCCharacterBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Movement")
	bool bShouldMove;
	//void SetShouldMove(); //캐릭터에서 Current bShouldMove를 가져오기만 함
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	FVector Velocity;
	//void SetVelocity();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	float GroundSpeed;
	//void SetGroundSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	uint8 bIsFalling : 1;
	//void SetIsFalling();
};
