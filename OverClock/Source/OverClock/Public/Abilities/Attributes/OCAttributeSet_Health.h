#pragma once

#include "CoreMinimal.h"
#include "Abilities/Attributes/OCAttributeSetBase.h"
#include "OCAttributeSet_Health.generated.h"


UCLASS()
class OVERCLOCK_API UOCAttributeSet_Health : public UOCAttributeSetBase
{
	GENERATED_BODY()

public:
	UOCAttributeSet_Health(){}

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRef_Health)
	FGameplayAttributeData Health;
	OC_ATTRIBUTE_ACCESSORS(UOCAttributeSet_Health, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	OC_ATTRIBUTE_ACCESSORS(UOCAttributeSet_Health, MaxHealth)
	
	UFUNCTION()
	void OnRef_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	void HandleHealthChanged(const FGameplayEffectModCallbackData& Data);
	
};
