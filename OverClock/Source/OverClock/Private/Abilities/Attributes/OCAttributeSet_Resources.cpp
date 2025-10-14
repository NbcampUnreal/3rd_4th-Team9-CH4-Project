#include "Abilities/Attributes/OCAttributeSet_Resources.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UOCAttributeSet_Resources::UOCAttributeSet_Resources()
{
	InitUltCharge(1.f);
	InitMaxUltCharge(1.f);
}

void UOCAttributeSet_Resources::OnRep_UltCharge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOCAttributeSet_Resources, UltCharge, OldValue);
}

void UOCAttributeSet_Resources::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UOCAttributeSet_Resources, UltCharge, COND_None, REPNOTIFY_Always);
}

void UOCAttributeSet_Resources::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetUltChargeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 100.f);
	}
}

void UOCAttributeSet_Resources::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetUltChargeAttribute())
	{
		const float Clamped = FMath::Clamp(UltCharge.GetCurrentValue(), 0.f, 100.f);
		UltCharge.SetCurrentValue(Clamped);
		UltCharge.SetBaseValue(Clamped);
	}
}
