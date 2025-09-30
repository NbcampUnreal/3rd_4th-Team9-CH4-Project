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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill_Active, "InputTag.Skill.Active") 
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact")// E
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ultimate, "InputTag.Ultimate")   // Q		UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact")   // F/V 등
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Skill1, "InputTag.Attack.Skill1")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Skill2, "InputTag.Attack.Skill2")

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
	UE_DEFINE_GAMEPLAY_TAG(Ability_RangedAttack, "Ability.RangedAttack")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Reload, "Ability.Reload")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill1, "Ability.Skill1")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill2, "Ability.Skill2")

	/*State Tags*/
	UE_DEFINE_GAMEPLAY_TAG(State_Alive, "State.Alive")
	UE_DEFINE_GAMEPLAY_TAG(State_Marked, "State.Marked")
	UE_DEFINE_GAMEPLAY_TAG(State_ActionLock, "State.ActionLock")
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead")
	UE_DEFINE_GAMEPLAY_TAG(State_ActionLock_QuickStep, "State.ActionLock.QuickStep")
	UE_DEFINE_GAMEPLAY_TAG(State_ActionLock_RangedAttack, "State.ActionLock.RangedAttack")
	UE_DEFINE_GAMEPLAY_TAG(State_ActionLock_Reload, "State.ActionLock.Reload")
	UE_DEFINE_GAMEPLAY_TAG(State_Twin_SpecialAmmo, "State.Twin.SpecialAmmo") // 윈도우 태그

	/*Effect Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Effect_Mark, "Effect.Mark")

	//Cooldown Tag
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Rev_DeathBullet, "Cooldown.Rev.DeathBullet")
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Twin_QuickStep, "Cooldown.Twin.QuickStep")
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Twin_VoltexGrenade, "Cooldown.Twin.VoltexGrenade")

	//cue Tags
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_MuzzleFlash, "GameplayCue.Weapon.MuzzleFlash")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Reload, "GameplayCue.Weapon.Reload")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Reload_Rev, "GameplayCue.Weapon.Reload.Rev")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Reload_Twin, "GameplayCue.Weapon.Reload.Twin")


	UE_DEFINE_GAMEPLAY_TAG(Data_Cooldown_Duration, "Data.Duration.Cooldown")// 쿨타임 set by caller 키
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage")

	UE_DEFINE_GAMEPLAY_TAG(Event_Spawn_Grenade, "Event.Spawn.Grenade")
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_Died, "Event.Character.Died")
}
