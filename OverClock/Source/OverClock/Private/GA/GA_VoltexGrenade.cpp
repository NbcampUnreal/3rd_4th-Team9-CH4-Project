#include "GA/GA_VoltexGrenade.h"

#include "Player/OCCharacterBase.h"
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"

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

    const AOCCharacterBase* Char = Cast<AOCCharacterBase>(ActorInfo->AvatarActor.Get());

    const FGameplayTag CharTypeTag = Char->GetCurrentTag();

    //Animation
    const FOCAnimStruct* AS = Char->GetAnimDataAsset()->CharacterAnimations.Find(CharTypeTag);

    UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->Skill2, MontageSlot, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, true, 1.f, 0.f);

    Task->OnCompleted.AddDynamic(this, &UGA_VoltexGrenade::OnMontageCompleted);

    Task->OnBlendOut.AddDynamic(this, &UGA_VoltexGrenade::OnMontageCompleted);

    Task->OnInterrupted.AddDynamic(this, &UGA_VoltexGrenade::OnMontageInterrupted);

    Task->OnCancelled.AddDynamic(this, &UGA_VoltexGrenade::OnMontageInterrupted);

    Task->ReadyForActivation();

    const FGameplayTag SpawnTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Spawn.Grenade"));

    UAbilityTask_WaitGameplayEvent * WaitEvt = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SpawnTag, nullptr, true, true);

    WaitEvt->EventReceived.AddDynamic(this, &UGA_VoltexGrenade::OnGrenadeSpawnEvent);

    WaitEvt->ReadyForActivation();
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

    const AOCCharacterBase * Char = Cast<AOCCharacterBase>(CurrentActorInfo->AvatarActor.Get());

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