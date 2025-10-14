#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_HealEffect.generated.h"

UCLASS()
class OVERCLOCK_API UGE_HealEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UGE_HealEffect();
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer GrantedTags;

	
};
