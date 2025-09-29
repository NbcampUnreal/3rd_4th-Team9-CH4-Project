#include "OverClockTypes/OverClockStructTypes.h"
#include "Abilities/OCGameplayAbility.h"

bool FOCAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityClass != nullptr;
}
