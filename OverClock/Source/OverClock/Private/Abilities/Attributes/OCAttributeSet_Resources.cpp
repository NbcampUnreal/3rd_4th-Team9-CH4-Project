#include "Abilities/Attributes/OCAttributeSet_Resources.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UOCAttributeSet_Resources::UOCAttributeSet_Resources()
{
	InitUltCharge(0.f);
	InitMaxUltCharge(100.f);
}

void UOCAttributeSet_Resources::OnRep_UltCharge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet_Resources, UltCharge, OldValue);
}

void UOCAttributeSet_Resources::OnRep_MaxUltCharge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet_Resources, MaxUltCharge, OldValue);
}

void UOCAttributeSet_Resources::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet_Resources, UltCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet_Resources, MaxUltCharge,  COND_None, REPNOTIFY_Always);
}

void UOCAttributeSet_Resources::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxUltChargeAttribute())
	{
		AdjustAttributeForMaxChange(UltCharge, MaxUltCharge, NewValue, GetUltChargeAttribute());
		NewValue = FMath::Max(NewValue, 1.f);;
	}
}

void UOCAttributeSet_Resources::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetUltChargeAttribute())
	{
		const float Max = MaxUltCharge.GetCurrentValue();
		const float Clamped = FMath::Clamp(UltCharge.GetCurrentValue(), 0.f, Max);
		UltCharge.SetCurrentValue(Clamped);
		UltCharge.SetBaseValue(Clamped);
	}
}
