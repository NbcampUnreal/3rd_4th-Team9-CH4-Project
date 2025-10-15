#include "Abilities/Attributes/OCAttributeSet_Health.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Data/OCGameplayTags.h"

UOCAttributeSet_Health::UOCAttributeSet_Health()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	IncomingDamage.SetBaseValue(0.f);
	IncomingDamage.SetCurrentValue(0.f);
}

void UOCAttributeSet_Health::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet_Health, Health, OldValue);
}

void UOCAttributeSet_Health::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet_Health, MaxHealth, OldValue);
}

void UOCAttributeSet_Health::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet_Health, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet_Health, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UOCAttributeSet_Health::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void UOCAttributeSet_Health::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float Damage = IncomingDamage.GetCurrentValue();
		if (Damage > 0.f)
		{
			const float NewHP = FMath::Clamp(Health.GetCurrentValue() - Damage, 0.f, MaxHealth.GetCurrentValue());
			Health.SetCurrentValue(NewHP);
			Health.SetBaseValue(NewHP);
			
			IncomingDamage.SetCurrentValue(0.f);
			IncomingDamage.SetBaseValue(0.f);
			
			if (NewHP <= 0.f)
			{
				if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
				{
					ASC->AddLooseGameplayTag(OCGameplayTags::State_Dead);
					ASC->RemoveLooseGameplayTag(OCGameplayTags::State_Alive);
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		HandleHealthChanged(Data);
	}
}

void UOCAttributeSet_Health::HandleHealthChanged(const FGameplayEffectModCallbackData& Data)
{
	const float Max = MaxHealth.GetCurrentValue();
	const float Clamped = FMath::Clamp(Health.GetCurrentValue(), 0.f, Max);

	Health.SetCurrentValue(Clamped);
	Health.SetBaseValue(Clamped);
}
