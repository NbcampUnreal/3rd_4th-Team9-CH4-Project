// #include "Abilities/OCAbilitySet.h"
//
// #include "AbilitySystemComponent.h"
//
// void FOCAblitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
// {
// 	if (Handle.IsValid())
// 	{
// 		AbilitySpecHandles.Add(Handle);
// 	}
// }
//
// UOCAbilitySet::UOCAbilitySet(const FObjectInitializer& ObjectInitializer)
// {
// }
//
// void UOCAbilitySet::GiveToAbilitySystem
// 		(UAbilitySystemComponent* ASC, FOCAblitySet_GrantedHandles* OutGrantedHandles,UObject* SourceObject)
// {
// 	check(ASC);
// 	if (ASC->IsOwnerActorAuthoritative())
// 	{
// 		return;
// 	}
// 	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
// 	{
// 		const FOCData_GA& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
// 		if (!IsValid(AbilityToGrant.Ability))
// 		{
// 			continue;
// 		}
// 		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();
//
// 		FGameplayAbilitySpec AbilitySpec(AbilityCDO);
// 		AbilitySpec.SourceObject = SourceObject;
// 		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
//
// 		//Handle은 int32라서 배열에 넣어서 쉽게 사용할 수 있음
// 		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
// 		if (OutGrantedHandles)
// 		{
// 			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
// 		}
// 	}
// }
