#include "GA/GA_VoltexGrenade.h"

#include "Player/OCCharacterBase_V2.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Abilities/VoltexGrenadeProjectile.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_VoltexGrenade::UGA_VoltexGrenade()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_VoltexGrenade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FGameplayTag SpawnTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Spawn.Grenade"));

    UAbilityTask_WaitGameplayEvent * WaitEvt = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SpawnTag, nullptr, true, true);

    WaitEvt->EventReceived.AddDynamic(this, &UGA_VoltexGrenade::OnGrenadeSpawnEvent);

    WaitEvt->ReadyForActivation();

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

void UGA_VoltexGrenade::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_VoltexGrenade::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_VoltexGrenade::Server_SpawnGrenade_Implementation(FVector_NetQuantize Start, FVector_NetQuantizeNormal Dir)
{
    FActorSpawnParameters SP;

    SP.Owner = CurrentActorInfo->AvatarActor.Get();

    SP.Instigator = Cast<APawn>(SP.Owner);

    SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AVoltexGrenadeProjectile* VGP = GetWorld()->SpawnActor<AVoltexGrenadeProjectile>(ProjectileClass, FTransform(Dir.Rotation(), Start), SP);
}

void UGA_VoltexGrenade::OnGrenadeSpawnEvent(FGameplayEventData Payload)
{
    if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() || !ProjectileClass)
    {
        return;
    }

    const AOCCharacterBase_V2 * Char = Cast<AOCCharacterBase_V2>(CurrentActorInfo->AvatarActor.Get());

    FVector Start = FVector::ZeroVector;

    FVector Dir = FVector::ForwardVector;

    if (USkeletalMeshComponent* Mesh = Char->GetMesh())
    {
      Start = Mesh->GetSocketLocation(SpawnSocket);
    }
  
    if (APlayerController* PC = Cast<APlayerController>(CurrentActorInfo->PlayerController.Get()))
    {
        FVector CamLoc; FRotator CamRot;

        PC->GetPlayerViewPoint(CamLoc, CamRot);

        Dir = CamRot.Vector();

        if (Start.IsZero())
        {
            Start = CamLoc + Dir * 50.f;
        }
    }
    else
    {
        Dir = Char->GetActorForwardVector();

        if (Start.IsZero())
        {
            Start = Char->GetActorLocation() + Dir * 50.f;
        }
    }
    
    Server_SpawnGrenade(Start, Dir.GetSafeNormal());
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

UAbilityTask_PlayMontageAndWait* UGA_VoltexGrenade::PlayMontageTask(UAnimMontage* Montage, float PlayRate,
    FName StartSection, bool bStopWhenAbilityEnds, float RootMotionScale, float StartTimeSeconds,
    bool bAllowInterruptAfterBlendOut) const
{
    if (!Montage) return nullptr;

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        const_cast<UGA_VoltexGrenade*>(this),
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