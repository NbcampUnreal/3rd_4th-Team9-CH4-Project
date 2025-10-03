#include "GA/GA_DeathBullet.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Component/WeaponAmmoComponent.h"

// 캐릭터별 장전 모션 데이터(예시)
#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"

UGA_DeathBullet::UGA_DeathBullet()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_DeathBullet::ActivateAbility(
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

	const AOCRevenant* Rev = CastChecked<AOCRevenant>(ActorInfo->AvatarActor.Get());

	const FOCAnimStruct* AS = Rev->GetAnimDataAsset()->CharacterAnimations.Find(Rev->GetCharacterTypeTag());

	UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->Skill1, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

	auto* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, true);

	Task->OnBlendOut.AddDynamic(this, &UGA_DeathBullet::OnMontageCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_DeathBullet::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_DeathBullet::OnMontageInterrupted);
	Task->OnCancelled.AddDynamic(this, &UGA_DeathBullet::OnMontageInterrupted);
	Task->ReadyForActivation();
}

void UGA_DeathBullet::OnMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("[DeathBullet] Montage Completed -> EndAbility"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DeathBullet::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("[DeathBullet] Montage Interrupted -> EndAbility"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

