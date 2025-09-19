#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimSequenceBase.h"
#include "OCAnimStruct.generated.h"

USTRUCT(BlueprintType)
struct FOCAnimStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> RunAnimBS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> JogFwdStartAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> JogFwdStopAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> JumpRecoveryAnimAdd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> JumpStartAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> JumpApexAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> JumpPreLandAnim;
	
	// 필요한 다른 애니메이션들도 추가
};