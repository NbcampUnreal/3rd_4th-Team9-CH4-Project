#include "Data/OCGameplayTags.h"

namespace OCGameplayTags
{
	/* Input Tags*/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump")
	
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

	/*State Tags*/
	UE_DEFINE_GAMEPLAY_TAG(State_Alive, "State.Alive")
	UE_DEFINE_GAMEPLAY_TAG(State_Marked, "State.Marked")

	/*Effect Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Effect_Mark, "Effect.Mark")
}
