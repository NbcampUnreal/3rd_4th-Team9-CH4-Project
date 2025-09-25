// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/PeacekeeperCooldown.h"

UPeacekeeperCooldown::UPeacekeeperCooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.6f));
    InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.PeaceKeeper")));
}
