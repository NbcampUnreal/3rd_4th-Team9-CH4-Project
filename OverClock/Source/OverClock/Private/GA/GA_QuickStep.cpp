#include "GA/GA_QuickStep.h"

#include "Player/OCCharacterBase.h" 
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"

UGA_QuickStep::UGA_QuickStep()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_QuickStep::ActivateAbility(
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

UAnimSequence* UGA_QuickStep::ChooseSequenceForDirection(const ACharacter* Char, const FVector& WorldDir) const
{
    const FRotator InvYaw(0.f, -Char->GetActorRotation().Yaw, 0.f);
    const FVector Local = InvYaw.RotateVector(WorldDir).GetSafeNormal2D();

    const float Deg = FMath::RadiansToDegrees(FMath::Atan2(Local.Y, Local.X));
    auto InRange = [](float A, float Min, float Max)
        {
            return (A >= Min && A < Max);
        };

    if (bUseEightWay)
    {
        if (InRange(Deg, -22.5f, 22.5f))  return Seq_F;
        if (InRange(Deg, 67.5f, 112.5f))  return Seq_R;
        if (Deg >= 157.5f || Deg < -157.5f) return Seq_B;
        if (InRange(Deg, -112.5f, -67.5f))  return Seq_L;
        return Seq_F;
    }
    else
    {
        if (InRange(Deg, -45.f, 45.f))   return Seq_F;
        if (InRange(Deg, 45.f, 135.f))   return Seq_R;
        if (InRange(Deg, -135.f, -45.f)) return Seq_L;
        return Seq_B;
    }
}

void UGA_QuickStep::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_QuickStep::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_QuickStep::EndAbility(
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

UAbilityTask_PlayMontageAndWait* UGA_QuickStep::PlayMontageTask(UAnimMontage* Montage, float PlayRate,
    FName StartSection, bool bStopWhenAbilityEnds, float RootMotionScale, float StartTimeSeconds,
    bool bAllowInterruptAfterBlendOut) const
{
    if (!Montage) return nullptr;

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        const_cast<UGA_QuickStep*>(this),
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