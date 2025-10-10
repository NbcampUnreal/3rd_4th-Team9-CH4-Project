#pragma once

#include "CoreMinimal.h"
#include "OverClockTypes/OverClockStructTypes.h"
#include "DA_OCHeroStartUpData.generated.h"

class UOCAbilitySystemComponent;
class UOCGameplayAbility;
class UGameplayEffect;

UCLASS()
class OVERCLOCK_API UDA_OCHeroStartUpData : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OC|StartUpData")
	void GiveToAbilitySystemComponent(UOCAbilitySystemComponent* ASC, int32 ApplyLevel = 1) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="StartUpData")
	TArray<TSubclassOf<UOCGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category="StartUpData")
	TArray<TSubclassOf<UOCGameplayAbility>> ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category="StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;

private:
	UPROPERTY(EditDefaultsOnly, Category="StartUpData")
	TArray<FOCAbilitySet> HeroStartUpAbilitySets;
	
	void GrantAbilities(const TArray<TSubclassOf<UOCGameplayAbility>>& InAbilitiesToGive, UOCAbilitySystemComponent* ASC, int32 ApplyLevel) const;
	
};
