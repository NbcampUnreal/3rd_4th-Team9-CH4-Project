#include "GA/GA_RangedAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "GA/GA_Reload.h"


UGA_RangedAttack::UGA_RangedAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_RangedAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    CommitAbility(Handle, ActorInfo, ActivationInfo);

    //GameplayCue 
    const ACharacter* Char = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

    FGameplayCueParameters Params;

    Params.SourceObject = Char->GetMesh(); // GC_Fire가 이 컴포넌트에 Muzzle 소켓으로 붙여서 스폰

    ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.MuzzleFlash")), Params);
  
    //Animation
    const AOCRevenant* Rev = CastChecked<AOCRevenant>(ActorInfo->AvatarActor.Get());

    const FOCAnimStruct* S = Rev->GetAnimDataAsset()->CharacterAnimations.Find(Rev->GetCharacterTypeTag());

    UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(S->PrimaryAttack, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

    // Cooldown until animation end
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, DynamicMontageSlotName, /*bStopWhenAbilityEnds=*/true, 1.f, 0.f);

    Task->OnCompleted.AddDynamic(this, &UGA_RangedAttack::OnMontageCompleted);

    Task->OnInterrupted.AddDynamic(this, &UGA_RangedAttack::OnMontageInterrupted);

    Task->OnCancelled.AddDynamic(this, &UGA_RangedAttack::OnMontageInterrupted);

    Task->ReadyForActivation();
}

void UGA_RangedAttack::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
}

void UGA_RangedAttack::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/true);
}

void UGA_RangedAttack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}