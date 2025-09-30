#include "Data/DA_OCHeroStartUpData.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Abilities/OCGameplayAbility.h"
#include "GameplayEffect.h"

void UDA_OCHeroStartUpData::GiveToAbilitySystemComponent(UOCAbilitySystemComponent* ASC, int32 ApplyLevel) const
{
	check(ASC);

	const AActor* Owner = ASC->GetOwner();
	if (!Owner || !Owner->HasAuthority()) { return; }

	GrantList<UOCGameplayAbility>(ActivateOnGivenAbilities, ASC, ApplyLevel);
	GrantList<UOCGameplayAbility>(ReactiveAbilities, ASC, ApplyLevel);

	for (const FOCAbilitySet& Set : HeroStartUpAbilitySets)
	{
		if (!Set.IsValid()) continue;

		FGameplayAbilitySpec Spec(Set.AbilityClass, ApplyLevel);
		Spec.SourceObject = ASC->GetAvatarActor();
		Spec.GetDynamicSpecSourceTags().AddTag(Set.InputTag);

		ASC->GiveAbility(Spec);
	}

	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
	{
		if (!EffectClass) continue;

		UGameplayEffect* CDO = EffectClass->GetDefaultObject<UGameplayEffect>();
		ASC->ApplyGameplayEffectToSelf(CDO, ApplyLevel, ASC->MakeEffectContext());
	}
	
}
