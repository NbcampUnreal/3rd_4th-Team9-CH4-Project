#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_MarkEffect.generated.h"

UCLASS(Blueprintable)
class OVERCLOCK_API UGE_MarkEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UGE_MarkEffect();
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer GrantedTags;
};
