#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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

	UFUNCTION()
	void SetCharacterTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<class UOCAnimDataAsset> CharacterDataAsset;
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGameplayTag CurrentCharacterTag;
	
	UPROPERTY()
	TObjectPtr<AOCCharacterBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Movement")
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	uint8 bIsAccelerating:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	float AimPitch;
};
