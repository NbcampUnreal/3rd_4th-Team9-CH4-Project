#include "Abilities/TwinBlast/GA_RangedAttack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Component/WeaponAmmoComponent.h"

#include "GameFramework/PlayerController.h" 
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "GameplayEffectTypes.h"
#include "Data/CharacterAttributeSet.h"

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
   
    PerformCameraTraceAndFire(Handle, ActorInfo, ActivationInfo);

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

void UGA_RangedAttack::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
}

void UGA_RangedAttack::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/true);
}

void UGA_RangedAttack::PerformCameraTraceAndFire(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    AActor* Avatar = ActorInfo->AvatarActor.Get();

    FVector CamLoc; 
    FRotator CamRot;

    if (const APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get()))
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else
    {
        CamLoc = Avatar->GetActorLocation();
        CamRot = Avatar->GetActorRotation();
    }

    const FVector TraceStart = CamLoc;
    const FVector TraceEnd = TraceStart + CamRot.Vector() * 20000.f;

    FCollisionQueryParams Params(SCENE_QUERY_STAT(GA_RangedAttack), true);
    Params.AddIgnoredActor(Avatar);
    Params.bReturnPhysicalMaterial = false;

    FCollisionObjectQueryParams ObjParams;
    ObjParams.AddObjectTypesToQuery(ECC_Pawn);

    FHitResult HitResult;
    const bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(HitResult, TraceStart, TraceEnd, ObjParams, Params);

#if !(UE_BUILD_SHIPPING)
    DrawDebugLine(GetWorld(), TraceStart, bBlockingHit ? HitResult.Location : TraceEnd,
        FColor::Red, false, 1.0f, 0, 0.5f);
#endif

    if (bBlockingHit)
    {
        FinalTargetLocation = HitResult.Location;

        if (APawn* HitPawn = Cast<APawn>(HitResult.GetActor()))
        {
            bHitTargetActor = true;
            UE_LOG(LogTemp, Warning, TEXT("Hit! GA_RangedAttack -> %s"), *HitPawn->GetName());

            if (ActorInfo->IsNetAuthority())
            {
                ApplyDamage_ServerSide(HitResult, *ActorInfo);
            }
            else
            {
                Server_ApplyDamage(HitResult);
            }
        }
        else
        {
            bHitTargetActor = false;
        }
    }
}

void UGA_RangedAttack::ApplyDamage_ServerSide(const FHitResult& Hit, const FGameplayAbilityActorInfo& Info)
{
    AActor* TargetActor = Hit.GetActor();

    if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Target: % s"), *TargetActor->GetName());

        FGameplayEffectContextHandle Ctx = Info.AbilitySystemComponent->MakeEffectContext();
        Ctx.AddHitResult(Hit);

        FGameplayEffectSpecHandle Spec = Info.AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), Ctx);
        if (Spec.IsValid() && Spec.Data.IsValid())
        {
            Spec.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.Damage")), -FMath::Abs(BaseDamage));

            Info.AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);

            const float NewHealth = TargetASC->GetNumericAttribute(UCharacterAttributeSet::GetHealthAttribute());

            Client_OnDamageConfirmed(TargetActor, NewHealth, FMath::Abs(BaseDamage));
        }
    }
}

void UGA_RangedAttack::Server_ApplyDamage_Implementation(const FHitResult& Hit)
{
    check(CurrentActorInfo);
    ApplyDamage_ServerSide(Hit, *CurrentActorInfo);
}

void UGA_RangedAttack::Client_OnDamageConfirmed_Implementation(AActor* Target, float NewHealth, float AppliedDamage)
{
    UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Damage confirmed. Target=%s, Damage=%.1f, NewHealth=%.1f"), Target ? *Target->GetName() : TEXT("None"), AppliedDamage, NewHealth);
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

UAbilityTask_PlayMontageAndWait* UGA_RangedAttack::PlayMontageTask(UAnimMontage* Montage, float PlayRate,
    FName StartSection, bool bStopWhenAbilityEnds, float RootMotionScale, float StartTimeSeconds,
    bool bAllowInterruptAfterBlendOut) const
{
    if (!Montage) return nullptr;

    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        const_cast<UGA_RangedAttack*>(this),
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