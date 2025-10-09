#include "Data/DA_OCInputConfig.h"

bool FOCInputActionConfig::IsValid() const
{
	return InputTag.IsValid() && InputAction != nullptr;
}

UInputAction* UDA_OCInputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FOCInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}

UInputAction* UDA_OCInputConfig::FindAbilityInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FOCInputActionConfig& InputActionConfig : AbilityInputActions)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}
