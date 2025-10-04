#include "Data/OCGameplayTags.h"

namespace OCGameplayTags
{
#pragma region Input
	
	/* Input */
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,            "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,            "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint,          "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump,            "InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact,        "InputTag.Interact");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Primary,  "InputTag.Attack.Primary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Special,  "InputTag.Attack.Special");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill_Active,    "InputTag.Skill.Active");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ultimate,        "InputTag.Ultimate");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Reload,          "InputTag.Reload");

#pragma endregion

#pragma region Character
	
	/* Character */
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Revenant,  "Character.Type.Revenant");
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Twinblast, "Character.Type.Twinblast");
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Crunch,    "Character.Type.Crunch");
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Terra,     "Character.Type.Terra");
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Serath,    "Character.Type.Serath");
	UE_DEFINE_GAMEPLAY_TAG(Character_Type_Fey,       "Character.Type.Fey");

#pragma endregion
	
#pragma region Abilities

	/* Weapon / Attack */
	UE_DEFINE_GAMEPLAY_TAG(Ability_Weapon_Fire_Melee,      "Ability.Weapon.Fire.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Weapon_Fire_Hitscan,    "Ability.Weapon.Fire.Hitscan");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Weapon_Fire_Projectile, "Ability.Weapon.Fire.Projectile");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Weapon_Reload,          "Ability.Weapon.Reload");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Weapon_Overdrive,       "Ability.Weapon.Overdrive");

	/* Skill */
	UE_DEFINE_GAMEPLAY_TAG(Ability_Defense_Barrier,        "Ability.Defense.Barrier");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Defense_Block_Hold,     "Ability.Defense.Block.Hold");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Mobility_Dash,          "Ability.Mobility.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_DeadlyBullet,           "Ability.DeadlyBullet"); // (임시/특수기 명시)

	/* Ultimate */
	UE_DEFINE_GAMEPLAY_TAG(Ability_Ult_Earthshatter,  "Ability.Ult.Earthshatter");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Ult_GuardianOath,  "Ability.Ult.GuardianOath");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Ult_JudgmentSoul,  "Ability.Ult.JudgmentSoul");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Ult_Overdrive,     "Ability.Ult.Overdrive");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Ult_NatureTempest, "Ability.Ult.NatureTempest");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Ult_DivineAdvent,  "Ability.Ult.DivineAdvent");

#pragma endregion
	
#pragma region State
	
	/* State */
	UE_DEFINE_GAMEPLAY_TAG(State_Alive,             "State.Alive");
	UE_DEFINE_GAMEPLAY_TAG(State_Marked,            "State.Marked");
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Firing,     "State.Weapon.Firing");
	UE_DEFINE_GAMEPLAY_TAG(State_Mobility_Dashing,  "State.Mobility.Dashing");
	UE_DEFINE_GAMEPLAY_TAG(State_Defense_Blocking,  "State.Defense.Blocking");

	/* Cooldown */
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Weapon_Reload,  "Cooldown.Weapon.Reload");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Defense_Barrier,"Cooldown.Defense.Barrier");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Mobility_Dash,  "Cooldown.Mobility.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Control_Grenade,"Cooldown.Control.Grenade");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Control_Mark,   "Cooldown.Control.Mark");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Area_Zone,      "Cooldown.Area.Zone");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ult,            "Cooldown.Ult");

#pragma endregion
	
#pragma region Effect

	/* Effect */
	UE_DEFINE_GAMEPLAY_TAG(Effect_Buff_Shield,      "Effect.Buff.Shield");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Buff_Overdrive,   "Effect.Buff.Overdrive");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Debuff_Mark,      "Effect.Debuff.Mark");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Debuff_Slow,      "Effect.Debuff.Slow");
	UE_DEFINE_GAMEPLAY_TAG(Effect_CC_Root,          "Effect.CC.Root");
	UE_DEFINE_GAMEPLAY_TAG(Effect_CC_Stun,          "Effect.CC.Stun");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Heal_Instant,     "Effect.Heal.Instant");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Heal_HoT,         "Effect.Heal.HoT");

#pragma endregion
	
#pragma region GameplayCue

	/* GameplayCue */
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Melee_Swing,             "GameplayCue.Melee.Swing");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Impact_Heavy,            "GameplayCue.Impact.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_MuzzleFlash,      "GameplayCue.Weapon.MuzzleFlash");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Defense_Barrier_Deploy,  "GameplayCue.Defense.Barrier.Deploy");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Defense_Barrier_Break,   "GameplayCue.Defense.Barrier.Break");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Mobility_Dash_Start,     "GameplayCue.Mobility.Dash.Start");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Mobility_Dash_End,       "GameplayCue.Mobility.Dash.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Ult_Earthshatter_Jump,   "GameplayCue.Ult.Earthshatter.Jump");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Ult_Earthshatter_Impact, "GameplayCue.Ult.Earthshatter.Impact");

#pragma endregion

	// Temporary
	UE_DEFINE_GAMEPLAY_TAG(Ability_PeaceKeeper,     "Ability.PeaceKeeper");
	UE_DEFINE_GAMEPLAY_TAG(Ability_RevReload,       "Ability.RevReload");
	UE_DEFINE_GAMEPLAY_TAG(State_Firing,            "State.Firing");
	UE_DEFINE_GAMEPLAY_TAG(State_RevReloading,      "State.RevReloading");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_PeaceKeeper,    "Cooldown.PeaceKeeper");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_RevReload,      "Cooldown.RevReload");
}
