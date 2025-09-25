#include "Data/OCGameplayTags.h"

namespace OCGameplayTags
{
	/* Input Tags*/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Primary, "InputTag.Attack.Primary")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Reload, "InputTag.Reload")

	/* Character Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Revenant, "Character.Type.Revenant")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Twinblast, "Character.Type.Twinblast")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Crunch, "Character.Type.Crunch")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Terra, "Character.Type.Terra")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Serath, "Character.Type.Serath")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Fey, "Character.Type.Fey")

	//temporary
	UE_DEFINE_GAMEPLAY_TAG(Ability_PeaceKeeper, "Ability.PeaceKeeper")
	UE_DEFINE_GAMEPLAY_TAG(Ability_RevReload, "Ability.RevReload")
	UE_DEFINE_GAMEPLAY_TAG(State_Firing, "State.Firing")
	UE_DEFINE_GAMEPLAY_TAG(State_RevReloading, "State.RevReloading")
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_PeaceKeeper, "Cooldown.PeaceKeeper")
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_RevReload, "Cooldown.RevReload")

	//cue
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_RevMuzzleFlash, "GameplayCue.Weapon.RevMuzzleFlash")
}
