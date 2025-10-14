#include "OverClockTypes/OverClockStructTypes.h"
#include "Abilities/GameplayAbility.h"

bool FOCAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant != nullptr;
}
