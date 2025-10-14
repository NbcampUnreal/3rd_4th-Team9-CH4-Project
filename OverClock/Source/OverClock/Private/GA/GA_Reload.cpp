// GE_Reload.cpp
#include "GA/GA_Reload.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Player/OCRevenant.h"
#include "Player/OCCharacterBase.h"
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"
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

	const AOCCharacterBase* Char = Cast<AOCCharacterBase>(ActorInfo->AvatarActor.Get());
	const FGameplayTag CharTypeTag = Char->GetCurrentTag();

	const FGameplayTag* ReloadCueTag = ReloadCueByType.Find(CharTypeTag);
	FGameplayCueParameters Params;
	Params.SourceObject = Char->GetMesh();
	ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(*ReloadCueTag, Params);
	
	//Animation
	const FOCAnimStruct* AS = Char->GetAnimDataAsset()->CharacterAnimations.Find(CharTypeTag);
	UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->Reload, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

	// 애니 끝날 때까지 능력은 "활성(active)" 상태로 유지 → BlockAbilitiesWithTag/ActivationOwnedTags가 살아있음
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, /*bStopWhenAbilityEnds=*/true, 1.f, 0.f);

	Task->OnCompleted.AddDynamic(this, &UGA_Reload::OnMontageCompleted);

	Task->OnBlendOut.AddDynamic(this, &UGA_Reload::OnMontageCompleted);

	Task->OnInterrupted.AddDynamic(this, &UGA_Reload::OnMontageInterrupted);

	Task->OnCancelled.AddDynamic(this, &UGA_Reload::OnMontageInterrupted);

	Task->ReadyForActivation();
}

void UGA_Reload::OnMontageCompleted()
{
	AOCRevenant* Rev = Cast<AOCRevenant>(CurrentActorInfo->AvatarActor.Get());
	if (CurrentActorInfo->IsNetAuthority())
	{
		if (UWeaponAmmoComponent* Ammo = Rev->WeaponAmmoComp)
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
		if (UWeaponAmmoComponent* Ammo = Rev->WeaponAmmoComp)
		{
			Ammo->SetCurrentAmmo();
		}
		Rev->Server_RequestReloadRefill();
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
