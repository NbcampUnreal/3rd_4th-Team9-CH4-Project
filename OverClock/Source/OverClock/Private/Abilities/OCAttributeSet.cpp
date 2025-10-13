// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/OCAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UOCAttributeSet::UOCAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(300.0f);
	InitDamage(0.0f);
	InitAttackPower(30.0f);
	InitDefense(0.0f);
}

void UOCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
}

void UOCAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet, Health, OldHealth);
}

void UOCAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet, MaxHealth, OldMaxHealth);
}

void UOCAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet, Damage, OldAttackPower);
}

void UOCAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet, Defense, OldDefense);
}

void UOCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UOCAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		SetDamage(0.f);

		if (DamageDone > 0.f)
		{
			const float NewHealth = GetHealth() - DamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		}
	}
}

void UOCAttributeSet::AdjustAttributeForMaxChange(
	const FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute,
	float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;

	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	const float CurrentValue = AffectedAttribute.GetCurrentValue();

	float NewDelta = 0.f;
	if (CurrentMaxValue != 0.f)
	{
		const float CurrentPercent = CurrentValue / CurrentMaxValue;
		const float NewValue = CurrentPercent * NewMaxValue;
		NewDelta = NewValue - CurrentValue;
	}

	ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
}