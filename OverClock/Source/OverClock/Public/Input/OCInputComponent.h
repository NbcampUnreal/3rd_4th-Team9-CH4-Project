#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Data/DA_OCInputConfig.h"
#include "OCInputComponent.generated.h"

struct FGameplayTag;

UCLASS()
class OVERCLOCK_API UOCInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UDA_OCInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);

	template<class UserObject, typename CallbackFunc>
	void BindAbilityInputAction(const UDA_OCInputConfig* InInputConfig, UserObject* ContextObject, CallbackFunc PressedFunc, CallbackFunc ReleasedFunc);

};

template <class UserObject, typename CallbackFunc>
void UOCInputComponent::BindNativeInputAction(const UDA_OCInputConfig* InInputConfig,
	const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig, TEXT("Input config data asset is null"));
	if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}

template <class UserObject, typename CallbackFunc>
void UOCInputComponent::BindAbilityInputAction(const UDA_OCInputConfig* InInputConfig, UserObject* ContextObject,
	CallbackFunc PressedFunc, CallbackFunc ReleasedFunc)
{
	checkf(InInputConfig, TEXT("Input config data asset is null"));
	for (const FOCInputActionConfig& AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid()) continue;

		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, PressedFunc, AbilityInputActionConfig.InputTag);
		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, ReleasedFunc, AbilityInputActionConfig.InputTag);
	}
}
