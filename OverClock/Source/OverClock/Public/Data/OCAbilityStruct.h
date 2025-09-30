#pragma once

#include "CoreMinimal.h"
//#include "OCGameplayTags.h"
#include "GameplayTagContainer.h"
#include "OCAbilityStruct.generated.h"

USTRUCT(BlueprintType)
struct FOCAbilityStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag_Attack_Primary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag_Reload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag_Attack_Alt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag_Skill_Active;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag_Ultimate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag_Interact;
};
