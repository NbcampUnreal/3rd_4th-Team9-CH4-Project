#include "Data/DA_OCInputConfig.h"

UInputAction* UDA_OCInputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}