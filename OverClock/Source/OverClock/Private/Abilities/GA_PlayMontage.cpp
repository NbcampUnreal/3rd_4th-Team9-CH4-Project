#include "Abilities/GA_PlayMontage.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_PlayMontage::UGA_PlayMontage()
{
	bServerRespectsRemoteAbilityCancellation = false;
}

void UGA_PlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (AbilityMontage)
	{
		Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, AbilityMontage);

		Task->OnCompleted.AddDynamic(this, &UGA_PlayMontage::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_PlayMontage::OnMontageCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_PlayMontage::OnMontageCancelled);
		Task->OnBlendOut.AddDynamic(this, &UGA_PlayMontage::OnMontageCancelled);
		
		Task->ReadyForActivation();
	}
	
	// 5초 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(AbilityTimerHandle, this, 
		&UGA_PlayMontage::OnTimerExpired, 5.0f, false);
}

void UGA_PlayMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AbilityTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AbilityTimerHandle);
	}
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UGA_PlayMontage::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_PlayMontage::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_PlayMontage::OnTimerExpired()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}