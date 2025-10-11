#include "GA/GA_VoltexGrenade.h"

#include "Player/OCCharacterBase.h"
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"

UGA_VoltexGrenade::UGA_VoltexGrenade()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_VoltexGrenade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const AOCCharacterBase* Char = Cast<AOCCharacterBase>(ActorInfo->AvatarActor.Get());

    const FGameplayTag CharTypeTag = Char->GetCurrentTag();

    //Animation
    const FOCAnimStruct* AS = Char->GetAnimDataAsset()->CharacterAnimations.Find(CharTypeTag);

    UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->Skill2, MontageSlot, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

    // 애니 끝날 때까지 능력은 "활성(active)" 상태로 유지 → BlockAbilitiesWithTag/ActivationOwnedTags가 살아있음
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, /*bStopWhenAbilityEnds=*/true, 1.f, 0.f);

    Task->OnCompleted.AddDynamic(this, &UGA_VoltexGrenade::OnMontageCompleted);

    Task->OnBlendOut.AddDynamic(this, &UGA_VoltexGrenade::OnMontageCompleted);

    Task->OnInterrupted.AddDynamic(this, &UGA_VoltexGrenade::OnMontageInterrupted);

    Task->OnCancelled.AddDynamic(this, &UGA_VoltexGrenade::OnMontageInterrupted);

    Task->ReadyForActivation();
}

void UGA_VoltexGrenade::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_VoltexGrenade::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_VoltexGrenade::EndAbility(
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