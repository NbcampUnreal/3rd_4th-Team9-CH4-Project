#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "OCAbilitySystemComponent.generated.h"

UCLASS()
class OVERCLOCK_API UOCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OC|ASC")
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "OC|ASC")
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);
	
};
