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

    ACharacter* Char =  Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Char)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FVector Dir = GetQuickStepDirection(Char);

    if (bFaceInputDirection && !Dir.IsNearlyZero())
    {
        const FRotator NewYaw(0.f, Dir.Rotation().Yaw, 0.f);
        Char->SetActorRotation(NewYaw);
    }

    StartMoveTask(Char, Dir);

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
    if (MoveTask)
    {
        MoveTask->EndTask();
        MoveTask = nullptr;
    }

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

FVector UGA_QuickStep::GetQuickStepDirection(const ACharacter* Char) const
{
    if (!Char) return FVector::ZeroVector;

    FVector Dir = Char->GetLastMovementInputVector();
    Dir.Z = 0.f;

    if (Dir.IsNearlyZero())
    {
        Dir = Char->GetVelocity();
        Dir.Z = 0.f;
    }

    if (Dir.IsNearlyZero())
    {
        const APawn* Pawn = Cast<APawn>(Char);
        const FRotator ControlYaw(0.f, Pawn && Pawn->GetController() ? Pawn->GetController()->GetControlRotation().Yaw : Char->GetActorRotation().Yaw, 0.f);
        Dir = ControlYaw.Vector();
    }

    return Dir.GetSafeNormal2D();
}

void UGA_QuickStep::StartMoveTask(ACharacter* Char, const FVector& Dir)
{
    if (!Char || Dir.IsNearlyZero())
        return;

    const FVector StartLoc = Char->GetActorLocation();
    const FVector TargetLoc = StartLoc + Dir * RollDistance;

    float Duration = RollDuration > 0.f ? RollDuration : (RollDistance / FMath::Max(1.f, RollSpeed));

    MoveTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
        this,
        NAME_None,
        TargetLoc,
        Duration,
        false,
        EMovementMode::MOVE_Walking,
        false,
        nullptr,
        ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
        FVector::ZeroVector,
        false
    );

    if (MoveTask)
    {
        MoveTask->OnTimedOut.AddDynamic(this, &ThisClass::OnMoveTimedOut);
        MoveTask->OnTimedOutAndDestinationReached.AddDynamic(this, &ThisClass::OnMoveReachedDestination);
        MoveTask->ReadyForActivation();
    }
}

void UGA_QuickStep::OnMoveEnded()
{
    // 필요한 경우 여기서 후처리(예: 태그 해제/다음 상태 전환)
}

void UGA_QuickStep::OnMoveTimedOut()
{
    OnMoveEnded();
}

void UGA_QuickStep::OnMoveReachedDestination()
{
    OnMoveEnded();
}