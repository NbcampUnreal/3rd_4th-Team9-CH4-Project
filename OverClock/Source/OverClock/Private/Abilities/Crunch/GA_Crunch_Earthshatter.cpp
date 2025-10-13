#include "Abilities/Crunch/GA_Crunch_Earthshatter.h"
#include "GameFramework/Character.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Data/OCGameplayTags.h"

bool UGA_Crunch_Earthshatter::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bParentOK = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	if (!bParentOK) return false;

	if (!UltimateReadyTag.IsValid())
	{
		return true;
	}

	const UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC == nullptr)
	{
		return false;
	}

	return ASC->HasMatchingGameplayTag(UltimateReadyTag);
}

void UGA_Crunch_Earthshatter::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (UltimateReadyTag.IsValid())
	{
		RemoveLooseTag(UltimateReadyTag);
	}
}

void UGA_Crunch_Earthshatter::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	AddLooseTag(OCGameplayTags::State_Ultimate_Active);

	if (Montage != nullptr)
	{
		PlayMontageTask(Montage, 1.f);
	}

	if (IsServerAuthority())
	{
		ACharacter* C = GetOCCharacter();
		if (C != nullptr)
		{
			const FVector Center = C->GetActorLocation();
			SphereTraceAndApplyToTargets_Server(Center, Radius, DamageGE, StunGE);
		}
	}

	RemoveLooseTag(OCGameplayTags::State_Ultimate_Active);
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
