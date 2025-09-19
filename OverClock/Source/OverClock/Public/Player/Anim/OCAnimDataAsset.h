#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OCAnimStruct.h"
#include "Engine/DataAsset.h"
#include "OCAnimDataAsset.generated.h"

UCLASS()
class OVERCLOCK_API UOCAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	TMap<FGameplayTag, FOCAnimStruct> CharacterAnimations;
};
