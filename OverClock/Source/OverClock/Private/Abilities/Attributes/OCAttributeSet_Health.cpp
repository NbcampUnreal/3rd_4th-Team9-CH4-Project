#include "Abilities/Attributes/OCAttributeSet_Health.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UOCAttributeSet_Health::UOCAttributeSet_Health()
{
	InitMaxHealth(1.f);
	InitHealth(1.f);
}

void UOCAttributeSet_Health::OnRef_Health(const FGameplayAttributeData& OldValue)
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
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
