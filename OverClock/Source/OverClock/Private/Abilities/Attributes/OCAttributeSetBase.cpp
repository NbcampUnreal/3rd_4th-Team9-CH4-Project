#include "Abilities/Attributes/OCAttributeSetBase.h"
#include "Abilities/OCAbilitySystemComponent.h"


void UOCAttributeSetBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UOCAbilitySystemComponent* UOCAttributeSetBase::GetOCASC() const
{
	return Cast<UOCAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UOCAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
                                                      const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProp)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const float CurrentMax = MaxAttribute.GetCurrentValue();

	if (ASC == nullptr || FMath::IsNearlyEqual(CurrentMax, NewMaxValue))
	{
		return;
	}

	const float Current = AffectedAttribute.GetCurrentValue();
	const float NewDelta = (CurrentMax > 0.f) ? (Current * NewMaxValue / CurrentMax - Current) : NewMaxValue;

	ASC->ApplyModToAttribute(AffectedAttributeProp, EGameplayModOp::Additive, NewDelta);
}
