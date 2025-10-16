#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_Reload.generated.h"

class UAnimMontage;
class UGameplayEffect;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class OVERCLOCK_API UGA_Reload : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Reload();

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
    UFUNCTION() 
    void OnMontageCompleted();

    UFUNCTION() 
    void OnMontageInterrupted();

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Montage")
    TObjectPtr<UAnimMontage> DynMontage = nullptr;

    UAbilityTask_PlayMontageAndWait* PlayMontageTask(UAnimMontage* Montage, float PlayRate = 1.f,
        FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float RootMotionScale = 1.f,
        float StartTimeSeconds = 0.f, bool bAllowInterruptAfterBlendOut = false) const;
};