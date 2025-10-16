#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_RangedAttack.generated.h"

class UOCAnimDataAsset;
class UAnimMontage;
class UGameplayEffect;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class OVERCLOCK_API UGA_RangedAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_RangedAttack();

protected:
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags,
        const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

private:
    UFUNCTION() 
    void OnMontageCompleted();
    UFUNCTION() 
    void OnMontageInterrupted();

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

protected:
    void PerformCameraTraceAndFire(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo);

    UPROPERTY(BlueprintReadOnly, Category = "Ability|Trace")
    FVector FinalTargetLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability|Trace")
    bool bHitTargetActor = false;

    UFUNCTION(Server, Reliable)
    void Server_ApplyDamage(const FHitResult& HitResult);

    void ApplyDamage_ServerSide(const FHitResult& Hit, const FGameplayAbilityActorInfo& Info);

    UFUNCTION(Client, Reliable)
    void Client_OnDamageConfirmed(AActor* Target, float NewHealth, float AppliedDamage);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
    float BaseDamage = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Barrier|Montage")
    TObjectPtr<UAnimMontage> DynMontage = nullptr;

    UAbilityTask_PlayMontageAndWait* PlayMontageTask(UAnimMontage* Montage, float PlayRate = 1.f, 
        FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float RootMotionScale = 1.f, 
        float StartTimeSeconds = 0.f, bool bAllowInterruptAfterBlendOut = false) const;
};
