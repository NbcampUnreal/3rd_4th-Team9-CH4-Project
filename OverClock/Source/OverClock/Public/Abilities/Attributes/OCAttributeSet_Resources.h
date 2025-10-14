#pragma once

#include "CoreMinimal.h"
#include "Abilities/Attributes/OCAttributeSetBase.h"
#include "OCAttributeSet_Resources.generated.h"

UCLASS()
class OVERCLOCK_API UOCAttributeSet_Resources : public UOCAttributeSetBase
{
	GENERATED_BODY()

public:
	UOCAttributeSet_Resources();

	UPROPERTY(BlueprintReadOnly, Category = "Resources", ReplicatedUsing=OnRep_UltCharge)
	FGameplayAttributeData UltCharge;
	OC_ATTRIBUTE_ACCESSORS(UOCAttributeSet_Resources, UltCharge)

	UPROPERTY(BlueprintReadOnly, Category = "Resources", ReplicatedUsing=OnRep_UltCharge)
	FGameplayAttributeData MaxUltCharge;
	OC_ATTRIBUTE_ACCESSORS(UOCAttributeSet_Resources, MaxUltCharge)

	UFUNCTION()
	void OnRep_UltCharge(const FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
};
