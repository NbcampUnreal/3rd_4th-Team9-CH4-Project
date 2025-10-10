#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/OCCharacterAnimInstance.h"
#include "OCCrunchAnimInstance.generated.h"

UCLASS()
class OVERCLOCK_API UOCCrunchAnimInstance : public UOCCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Crunch|State")
	bool bIsAttacking = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Crunch|State")
	bool bIsUsingBarrier = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Crunch|State")
	bool bIsDashing = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Crunch|State")
	bool bIsUltimate = false;

private:
	void UpdateGASState();
};