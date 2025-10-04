#include "Data/DA_OCHeroStartUpData.h"
#include "AbilitySystemComponent.h"
#include "Abilities/OCGameplayAbility.h"
#include "GameplayEffect.h"

void UDA_OCHeroStartUpData::GiveToAbilitySystemComponent(UAbilitySystemComponent* ASC, int32 ApplyLevel) const
{
	check(ASC);

	const AActor* Owner = ASC->GetOwner();
	if (!Owner || !Owner->HasAuthority()) { return; }

	

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

void UDA_OCHeroStartUpData::GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& List,
	UAbilitySystemComponent* ASC, int32 ApplyLevel) const
{
	if (!ASC || List.IsEmpty()) return;

	for (const TSubclassOf<UGameplayAbility>& Cls : List)
	{
		if (!IsValid(Cls)) continue;

		FGameplayAbilitySpec Spec(Cls, ApplyLevel);
		Spec.SourceObject = ASC->GetAvatarActor();
		ASC->GiveAbility(Spec);
	}
}

void UDA_OCHeroStartUpData::GrantTaggedAbilities(const TArray<TSubclassOf<FOCAbilitySet>>& Sets,
	UAbilitySystemComponent* ASC, int32 ApplyLevel) const
{
	
}

void UDA_OCHeroStartUpData::ApplyEffects(const TArray<TSubclassOf<UGameplayEffect>>& List, UAbilitySystemComponent* ASC,
	int32 ApplyLevel) const
{
}
