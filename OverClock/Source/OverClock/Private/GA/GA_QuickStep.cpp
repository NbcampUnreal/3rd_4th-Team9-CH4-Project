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

    AOCCharacterBase* Char = Cast<AOCCharacterBase>(ActorInfo->AvatarActor.Get());
    USkeletalMeshComponent* Mesh = Char->GetMesh();
    UAnimInstance* Anim = Mesh->GetAnimInstance();

    FVector Dir = FVector::ZeroVector;
    if (const UCharacterMovementComponent* Move = Char->GetCharacterMovement())
    {
        Dir = FVector(Move->GetLastInputVector().X, Move->GetLastInputVector().Y, 0.f);
    }
    if (Dir.IsNearlyZero())
    {
        const FVector LMIV = Char->GetLastMovementInputVector();
        Dir = FVector(LMIV.X, LMIV.Y, 0.f);
    }
    if (Dir.IsNearlyZero())
    {
        const FVector V = Char->GetVelocity();
        Dir = FVector(V.X, V.Y, 0.f);
    }
    if (Dir.IsNearlyZero())
    {
        Dir = FVector(Char->GetControlRotation().Vector().X, Char->GetControlRotation().Vector().Y, 0.f);
    }
    if (Dir.IsNearlyZero())
    {
        Dir = FVector(Char->GetActorForwardVector().X, Char->GetActorForwardVector().Y, 0.f);
    }
    Dir.Normalize();

    UAnimSequenceBase* UseSeq = ChooseSequenceForDirection(Char, Dir);

    UAnimMontage* DynMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(UseSeq, MontageSlot, 0.05f, 0.05f, PlayRate, 1, 0.f, 0.f);

    UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DynMontage, PlayRate, NAME_None, true, 1.f, 0.f, 0.f);

    PlayTask->OnCompleted.AddDynamic(this, &UGA_QuickStep::OnMontageCompleted);
    PlayTask->OnBlendOut.AddDynamic(this, &UGA_QuickStep::OnMontageCompleted);
    PlayTask->OnInterrupted.AddDynamic(this, &UGA_QuickStep::OnMontageInterrupted);
    PlayTask->OnCancelled.AddDynamic(this, &UGA_QuickStep::OnMontageInterrupted);
    PlayTask->ReadyForActivation();

    const FVector Start = Char->GetActorLocation();
    const FVector Target = Start + (Dir * RollDistance);

    UAbilityTask_ApplyRootMotionMoveToForce* MoveTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
        this, NAME_None,
        Target, RollDuration,
        false, EMovementMode::MOVE_Walking,
        false,
        nullptr,
        ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
        FVector::ZeroVector,
        0.f);

    MoveTask->ReadyForActivation();
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