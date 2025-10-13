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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Alt, "InputTag.Attack.Alt")    // RMB: 특수(보통 무기 보조/방벽 등)
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill_Active, "InputTag.Skill.Active")   // E
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ultimate, "InputTag.Ultimate")   // Q
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact")   // F/V 등

	/* Character Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Revenant, "Character.Type.Revenant")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Twinblast, "Character.Type.Twinblast")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Crunch, "Character.Type.Crunch")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Terra, "Character.Type.Terra")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Serath, "Character.Type.Serath")
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Fey, "Character.Type.Fey")

	/*Team Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Team_Red, "Team.Red")
	UE_DEFINE_GAMEPLAY_TAG(Team_Blue, "Team.Blue")

	/*Skill Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Ability_DeadlyBullet, "Ability.DeadlyBullet")
	UE_DEFINE_GAMEPLAY_TAG(Ability_HealWhip, "Ability.HealWhip")
	UE_DEFINE_GAMEPLAY_TAG(Ability_HealGem, "Ability.HealGem")
	UE_DEFINE_GAMEPLAY_TAG(Ability_PoisonSeed, "Ability.PoisonSeed")
	UE_DEFINE_GAMEPLAY_TAG(Ability_WildStorm, "Ability.WildStorm")

	/*State Tags*/
	UE_DEFINE_GAMEPLAY_TAG(State_Alive, "State.Alive")
	UE_DEFINE_GAMEPLAY_TAG(State_Marked, "State.Marked")
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Hidden, "State.Weapon.Hidden")
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Scaling, "State.Weapon.Scaling")
	UE_DEFINE_GAMEPLAY_TAG(State_Stun, "State.Stun")

	/*Effect Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Effect_Mark, "Effect.Mark")
	UE_DEFINE_GAMEPLAY_TAG(Effect_Fey_Heal, "Effect.Fey.Heal")
	UE_DEFINE_GAMEPLAY_TAG(Effect_Fey_Deal, "Effect.Fey.Deal")
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
