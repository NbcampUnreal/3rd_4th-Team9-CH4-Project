// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA/GA_SharedCooldownBase.h"
#include "GA_VoltexGrenade.generated.h"

class UAnimMontage;
class UAnimSequenceBase;

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
    FName MontageSlot = FName(TEXT("FullBody"));

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Anim")
    float PlayRate = 1.f;

    UFUNCTION() void OnMontageCompleted();

    UFUNCTION() void OnMontageInterrupted();
};
