#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OverClockStructTypes.generated.h"

class UOCGameplayAbility;

USTRUCT(BlueprintType)
struct FOCAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UOCGameplayAbility> AbilityToGrant;

	bool IsValid() const;
};
