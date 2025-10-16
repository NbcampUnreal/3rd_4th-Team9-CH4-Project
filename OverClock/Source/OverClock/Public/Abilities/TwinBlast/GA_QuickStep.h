// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/TwinBlast/GA_SharedCooldownBase.h"
#include "GA_QuickStep.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_ApplyRootMotionMoveToForce;

UCLASS()
class OVERCLOCK_API UGA_QuickStep : public UGA_SharedCooldownBase
{
	GENERATED_BODY()
	
public:
	UGA_QuickStep();

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
    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move")
    float RollDistance = 600.f;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move")
    float RollDuration = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move", meta = (ClampMin = "1.0"))
    float RollSpeed = 2400.f;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move")
    bool bFaceInputDirection = true;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Directional")
    bool bUseEightWay = true;

private:

    UFUNCTION() void OnMontageCompleted();
    UFUNCTION() void OnMontageInterrupted();

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Montage")
    TObjectPtr<UAnimMontage> DynMontage = nullptr;

    UAbilityTask_PlayMontageAndWait* PlayMontageTask(UAnimMontage* Montage, float PlayRate = 1.f,
        FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float RootMotionScale = 1.f,
        float StartTimeSeconds = 0.f, bool bAllowInterruptAfterBlendOut = false) const;

    FVector GetQuickStepDirection(const class ACharacter* Char) const;

    void StartMoveTask(class ACharacter* Char, const FVector& Dir);

    UPROPERTY()
    UAbilityTask_ApplyRootMotionMoveToForce* MoveTask = nullptr;

    UFUNCTION() 
    void OnMoveEnded();   

    UFUNCTION() 
    void OnMoveTimedOut();         
    UFUNCTION() 
    void OnMoveReachedDestination();
};
