#include "GameEffects/GE_MarkEffect.h"

#include "Abilities/OCRBMissile.h"
#include "Data/OCGameplayTags.h"

UGE_MarkEffect::UGE_MarkEffect()
{
	GrantedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Marked")));
	
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(10.0f); // 10초 지속

	StackingType = EGameplayEffectStackingType::AggregateBySource; //덮어쓰기
	StackLimitCount = 1;
}
	
	// GrantedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Effect.Mark")));


	// 표식 태그 부여
	// FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(FName("State.Marked"));
	// FGameplayTag EffectTag = FGameplayTag::RequestGameplayTag(FName("Effect.Mark"));


//마크 태그가 들어오면 ? 컴포넌트가 캐릭터의 머리 위에 딱!
