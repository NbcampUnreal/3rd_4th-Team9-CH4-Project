#include "Abilities/OCGameplayAbility.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"

UOCGameplayAbility::UOCGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UOCAbilitySystemComponent* UOCGameplayAbility::GetOCASC() const
{
	return Cast<UOCAbilitySystemComponent>(CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent : nullptr);
}

ACharacter* UOCGameplayAbility::GetOCCharacter() const
{
	return Cast<ACharacter>(GetAvatarActorFromActorInfo());
}

bool UOCGameplayAbility::IsServerAuthority() const
{
	return GetCurrentActorInfo() && GetCurrentActorInfo()->IsNetAuthority();
}

UAbilityTask_PlayMontageAndWait* UOCGameplayAbility::PlayMontageTask(UAnimMontage* Montage, float PlayRate,
	FName StartSection, bool bStopWhenAbilityEnds, float RootMotionScale, float StartTimeSeconds,
	bool bAllowInterruptAfterBlendOut) const
{
	if (!Montage) return nullptr;
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			const_cast<UOCGameplayAbility*>(this),
			NAME_None,
			Montage,
			PlayRate,
			StartSection,
			bStopWhenAbilityEnds,
			RootMotionScale,
			StartTimeSeconds,
			bAllowInterruptAfterBlendOut
		);

	if (Task)
	{
		Task->ReadyForActivation();
	}
	return Task;
}


void UOCGameplayAbility::ApplySelfGE_Server(TSubclassOf<UGameplayEffect> EffectClass) const
{
	if (!IsServerAuthority() || !EffectClass) return;

	UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC == nullptr) return;

	ASC->ApplyGameplayEffectToSelf(EffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, ASC->MakeEffectContext());
	
}

void UOCGameplayAbility::AddLooseTag(const FGameplayTag& Tag) const
{
	UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC != nullptr)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UOCGameplayAbility::RemoveLooseTag(const FGameplayTag& Tag) const
{
	UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC != nullptr)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

void UOCGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActivationPolicy == EOCAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UOCGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActivationPolicy == EOCAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}