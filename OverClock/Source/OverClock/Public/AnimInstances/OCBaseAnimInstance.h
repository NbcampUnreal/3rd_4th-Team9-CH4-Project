#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OCBaseAnimInstance.generated.h"


UCLASS(Abstract)
class OVERCLOCK_API UOCBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override{}
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override{}
	
};
