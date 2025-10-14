#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA/GA_SharedCooldownBase.h"
#include "GA_VoltexGrenade.generated.h"

class UAnimMontage;
class AVoltexGrenadeProjectile;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class OVERCLOCK_API UGA_VoltexGrenade : public UGA_SharedCooldownBase
{
	GENERATED_BODY()
	
public:
	UGA_VoltexGrenade();

protected:
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
    UPROPERTY(EditDefaultsOnly, Category = "Roll|Anim")
    FName MontageSlot = FName(TEXT("UpperBody"));

    UFUNCTION() 
    void OnMontageCompleted();

    UFUNCTION() 
    void OnMontageInterrupted();

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    TSubclassOf<AVoltexGrenadeProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Grenade")
    FName SpawnSocket = TEXT("");

    UFUNCTION(Server, Reliable)
    void Server_SpawnGrenade(FVector_NetQuantize Start, FVector_NetQuantizeNormal Dir);

    UFUNCTION()
    void OnGrenadeSpawnEvent(FGameplayEventData Payload);

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Montage")
    TObjectPtr<UAnimMontage> DynMontage = nullptr;

    UAbilityTask_PlayMontageAndWait* PlayMontageTask(UAnimMontage* Montage, float PlayRate = 1.f,
        FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float RootMotionScale = 1.f,
        float StartTimeSeconds = 0.f, bool bAllowInterruptAfterBlendOut = false) const;
};
