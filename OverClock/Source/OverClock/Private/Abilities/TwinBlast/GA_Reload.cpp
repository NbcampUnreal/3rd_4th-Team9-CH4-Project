// GE_Reload.cpp
#include "Abilities/TwinBlast/GA_Reload.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Player/OCTwinBlast.h"
#include "Component/WeaponAmmoComponent.h"

UGA_Reload::UGA_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Reload::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (DynMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* Task = PlayMontageTask(DynMontage))
		{
			Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		}
	}
}

void UGA_Reload::OnMontageCompleted()
{
	AOCTwinBlast* Twin = Cast<AOCTwinBlast>(CurrentActorInfo->AvatarActor.Get());
	if (CurrentActorInfo->IsNetAuthority())
	{
		if (UWeaponAmmoComponent* Ammo = Twin->WeaponAmmoComp)
		{
			const int32 Before = Ammo->CurrentAmmo;
			if (Before < Ammo->MaxAmmo) 
			{ 
				Ammo->RefillAmmo(); 
			}
		}
	}
	else
	{
		if (UWeaponAmmoComponent* Ammo = Twin->WeaponAmmoComp)
		{
			Ammo->SetCurrentAmmo();
		}
		Twin->Server_RequestReloadRefill();
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
}

void UGA_Reload::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
}

void UGA_Reload::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		const FGameplayTag Lock = FGameplayTag::RequestGameplayTag(TEXT("State.ActionLock"));
		if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Lock))
		{
			ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(Lock);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAbilityTask_PlayMontageAndWait* UGA_Reload::PlayMontageTask(UAnimMontage* Montage, float PlayRate,
	FName StartSection, bool bStopWhenAbilityEnds, float RootMotionScale, float StartTimeSeconds,
	bool bAllowInterruptAfterBlendOut) const
{
	if (!Montage) return nullptr;

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		const_cast<UGA_Reload*>(this),
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