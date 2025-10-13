#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "OCAttributeSetBase.generated.h"

#define OC_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS(Abstract)
class OVERCLOCK_API UOCAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "OC|AttributeSet")
	UOCAbilitySystemComponent* GetOCASC() const;
	
	
protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProp);

	static float Clamp0ToMax(float Value, float Max) {return FMath::Clamp(Value, 0.f, Max);}
	
};
