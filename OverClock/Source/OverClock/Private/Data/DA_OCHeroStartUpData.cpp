#include "Data/DA_OCHeroStartUpData.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/OCAbilitySystemComponent.h"

void UDA_OCHeroStartUpData::GiveToAbilitySystemComponent(UOCAbilitySystemComponent* ASC, int32 ApplyLevel) const
{
	check(ASC);

	GrantAbilities(ActivateOnGivenAbilities, ASC, ApplyLevel);
	GrantAbilities(ReactiveAbilities, ASC, ApplyLevel);
	
	
	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();

			ASC->ApplyGameplayEffectToSelf(EffectCDO, ApplyLevel, ASC->MakeEffectContext());
		}
	}

	for (const FOCAbilitySet& AbilitySet : HeroStartUpAbilitySets)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = ASC->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

		ASC->GiveAbility(AbilitySpec);
	}
	
}


void UDA_OCHeroStartUpData::GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilitiesToGive,
	UOCAbilitySystemComponent* ASC, int32 ApplyLevel) const
{
	if (!ASC || InAbilitiesToGive.IsEmpty()) return;

	for (const TSubclassOf<UGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!IsValid(Ability)) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = ASC->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		ASC->GiveAbility(AbilitySpec);
	}
}


