// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA/GA_SharedCooldownBase.h"
#include "GA_QuickStep.generated.h"

class UAnimMontage;
class UAnimSequenceBase;

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
    UPROPERTY(EditDefaultsOnly, Category = "Roll|Anim")
    FName MontageSlot = FName(TEXT("FullBody"));

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Anim")
    float PlayRate = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move")
    float RollDistance = 600.f;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move")
    float RollDuration = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Move")
    bool bFaceInputDirection = true;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Directional")
    bool bUseEightWay = true;

    UPROPERTY(EditDefaultsOnly, Category = "Roll|Directional") 
    UAnimSequence* Seq_F = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Roll|Directional") 
    UAnimSequence* Seq_B = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Roll|Directional") 
    UAnimSequence* Seq_L = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Roll|Directional") 
    UAnimSequence* Seq_R = nullptr;

private:
    UAnimSequence* ChooseSequenceForDirection(const class ACharacter* Char, const FVector& WorldDir) const;

    UFUNCTION() void OnMontageCompleted();
    UFUNCTION() void OnMontageInterrupted();
};
