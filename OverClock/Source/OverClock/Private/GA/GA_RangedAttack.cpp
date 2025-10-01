#include "GA/GA_RangedAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "GA/GA_Reload.h"
#include "Component/WeaponAmmoComponent.h"


UGA_RangedAttack::UGA_RangedAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGA_RangedAttack::CanActivateAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayTagContainer* SourceTags, 
    const FGameplayTagContainer* TargetTags, 
    FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_RangedAttack::ActivateAbility(
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

    AActor* Avatar = ActorInfo->AvatarActor.Get();

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

    UWeaponAmmoComponent* Ammo = Avatar->FindComponentByClass<UWeaponAmmoComponent>();

    if (ActorInfo->IsNetAuthority()) 
    {
        if (!Ammo || !Ammo->ConsumeAmmo(1)) 
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, /*rep=*/true, /*cancel=*/true);
            return;
        }
    }
    else 
    {
        if (!Ammo || Ammo->IsAmmoEmpty()) 
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, /*rep=*/true, /*cancel=*/true);
            return;
        }
    }
    
    const ACharacter* Char = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

    FGameplayCueParameters Params;

    Params.SourceObject = Char->GetMesh();

    ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.MuzzleFlash")), Params);
   
    //Animation
    const AOCRevenant* Rev = CastChecked<AOCRevenant>(ActorInfo->AvatarActor.Get());

    const FOCAnimStruct* AS = Rev->GetAnimDataAsset()->CharacterAnimations.Find(Rev->GetCharacterTypeTag());

    UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->PrimaryAttack, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

    // Cooldown until animation end
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, /*bStopWhenAbilityEnds=*/true, 1.f, 0.f);

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