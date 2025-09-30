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

	/*Skill Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Ability_DeadlyBullet, "Ability.DeadlyBullet")
	UE_DEFINE_GAMEPLAY_TAG(Ability_RangedAttack, "Ability.RangedAttack")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Reload, "Ability.Reload")

	/*State Tags*/
	UE_DEFINE_GAMEPLAY_TAG(State_Alive, "State.Alive")
	UE_DEFINE_GAMEPLAY_TAG(State_Marked, "State.Marked")
	UE_DEFINE_GAMEPLAY_TAG(State_ActionLock, "State.ActionLock")

	/*Effect Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Effect_Mark, "Effect.Mark")

	//temporary
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Reload, "Cooldown.Reload")
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_RangedAttack, "Cooldown.RangedAttack")

	//cue Tags
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_MuzzleFlash, "GameplayCue.Weapon.MuzzleFlash")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Reload, "GameplayCue.Weapon.Reload")
}
