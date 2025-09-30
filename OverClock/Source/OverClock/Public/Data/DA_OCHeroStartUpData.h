#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OverClockTypes/OverClockStructTypes.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "DA_OCHeroStartUpData.generated.h"

class UOCAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class OVERCLOCK_API UDA_OCHeroStartUpData : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OC|StartUpData")
	void GiveToAbilitySystemComponent(UOCAbilitySystemComponent* ASC, int32 ApplyLevel = 1) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="StartUp|Abilities")
	TArray<TSubclassOf<UOCGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category="StartUp|Abilities")
	TArray<TSubclassOf<UOCGameplayAbility>> ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category="StartUp|Effects")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category="StartUp|Bindings", meta=(TitleProperty="InputTag"))
	TArray<FOCAbilitySet> HeroStartUpAbilitySets;

private:
	template<typename TGA>
	static void GrantList(const TArray<TSubclassOf<TGA>>& List, UOCAbilitySystemComponent* ASC, int32 ApplyLevel)
	{
    	if (!ASC) return;
    
    	for (const TSubclassOf<TGA>& Cls : List)
    	{
    		if (!Cls) continue;
    		FGameplayAbilitySpec Spec(Cls, ApplyLevel);
    		Spec.SourceObject = ASC->GetAvatarActor();
    		ASC->GiveAbility(Spec);
    	}
    }
};
