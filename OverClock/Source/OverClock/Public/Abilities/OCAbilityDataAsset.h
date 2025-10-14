#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/OCAbilityStruct.h"
#include "Engine/DataAsset.h"
#include "OCAbilityDataAsset.generated.h"

UCLASS()
class OVERCLOCK_API UOCAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	TMap<FGameplayTag, FOCAbilityStruct> CharacterAbilityTags;
};
