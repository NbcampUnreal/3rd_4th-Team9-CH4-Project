// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_RevReload.h"

UGE_RevReload::UGE_RevReload()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.5f));
    InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.RevReload")));
}
