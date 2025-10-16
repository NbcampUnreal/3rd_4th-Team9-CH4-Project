// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffects/GE_RangedAttackCooldown.h"

UGE_RangedAttackCooldown::UGE_RangedAttackCooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.1f));
}
