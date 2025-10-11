#include "GA/GA_RangedAttack.h"
#include "GA/GA_Reload.h"

#include "Player/OCCharacterBase.h"  
#include "Player/Anim/OCAnimStruct.h"
#include "Player/Anim/OCAnimDataAsset.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Component/WeaponAmmoComponent.h"

#include "GameFramework/PlayerController.h" 
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

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
    
    const ACharacter* CharActor = Cast<ACharacter>(ActorInfo->AvatarActor.Get());

    FGameplayCueParameters Params;

    Params.SourceObject = CharActor->GetMesh();

    ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.MuzzleFlash")), Params);
   
    PerformCameraTraceAndFire(ActorInfo);
    //Animation
    const AOCCharacterBase* Char = Cast<AOCCharacterBase>(Avatar);

    const FOCAnimStruct* AS = Char->GetAnimDataAsset()->CharacterAnimations.Find(Char->GetCurrentTag());

    UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->PrimaryAttack, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f); 

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

void UGA_RangedAttack::PerformCameraTraceAndFire(const FGameplayAbilityActorInfo* ActorInfo)
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    UWorld* World = GetWorld();
    if (!World || !Avatar)
    {
        FinalTargetLocation = FVector::ZeroVector;
        return;
    }

    FVector CameraLoc;
    FRotator CameraRot;

    APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
    if (PC)
    {
        PC->GetPlayerViewPoint(CameraLoc, CameraRot);
    }
    else
    {
        CameraLoc = Avatar->GetActorLocation();
        CameraRot = Avatar->GetActorRotation();
    }

    FVector TraceStart = CameraLoc;
    FVector TraceDirection = CameraRot.Vector();
    float MaxDistance = 10000.0f;
    FVector TraceEnd = TraceStart + (TraceDirection * MaxDistance);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Avatar);
    Params.bTraceComplex = false;
    Params.bReturnPhysicalMaterial = false;

    FHitResult HitResult;
    bool bBlockingHit = World->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        Params
    );

    FColor DebugColor = IsA<UGA_RangedAttack>() ? FColor::Red : FColor::Blue;
    DrawDebugLine(World, TraceStart, bBlockingHit ? HitResult.Location : TraceEnd, DebugColor, false, 5.0f, 0, 1.0f);

    if (bBlockingHit)
    {
        FinalTargetLocation = HitResult.Location;

        if (HitResult.GetActor() != nullptr)
        {
            bHitTargetActor = true;

            UE_LOG(LogTemp, Warning,
                TEXT(" Hit! GA: %s, Hit Actor: %s"),
                *GetName(),
                *HitResult.GetActor()->GetName());
        }
        else
        {
            bHitTargetActor = false;
        }
    }
    else
    {
        FinalTargetLocation = TraceEnd;
        bHitTargetActor = false;
    }
}

void UGA_RangedAttack::EndAbility(
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