// GE_Reload.cpp
#include "GA/GA_Reload.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Player/OCRevenant.h"
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

	ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.Reload")));
	//Animation
	const AOCRevenant* Rev = CastChecked<AOCRevenant>(ActorInfo->AvatarActor.Get());

	const FOCAnimStruct* AS = Rev->GetAnimDataAsset()->CharacterAnimations.Find(Rev->GetCharacterTypeTag());

	UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->Reload, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

	// 애니 끝날 때까지 능력은 "활성(active)" 상태로 유지 → BlockAbilitiesWithTag/ActivationOwnedTags가 살아있음
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, /*bStopWhenAbilityEnds=*/true, 1.f, 0.f);

	Task->OnCompleted.AddDynamic(this, &UGA_Reload::OnMontageCompleted);

	Task->OnInterrupted.AddDynamic(this, &UGA_Reload::OnMontageInterrupted);

	Task->OnCancelled.AddDynamic(this, &UGA_Reload::OnMontageInterrupted);

	Task->ReadyForActivation();
}

void UGA_Reload::OnMontageCompleted()
{
	if (CurrentActorInfo->IsNetAuthority())
	{
		if (AOCRevenant* Rev = Cast<AOCRevenant>(CurrentActorInfo->AvatarActor.Get()))
		{
			if (UWeaponAmmoComponent* Ammo = Rev->WeaponAmmoComp)  // ← 캐시 포인터만 사용
			{
				int32 Before = Ammo->CurrentAmmo;
				Ammo->RefillAmmo(); // 내부에서 Max(=6)로 클램프
				UE_LOG(LogTemp, Log, TEXT("[Reload] %d -> %d / Max=%d"), Before, Ammo->CurrentAmmo, Ammo->MaxAmmo);
			}
			else { UE_LOG(LogTemp, Warning, TEXT("[Reload] WeaponAmmo null from Character cache")); }
		}
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
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
