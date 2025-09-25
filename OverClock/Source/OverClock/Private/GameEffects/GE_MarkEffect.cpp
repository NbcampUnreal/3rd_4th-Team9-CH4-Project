#include "GameEffects/GE_MarkEffect.h"

#include "Abilities/OCRBMissile.h"
#include "Data/OCGameplayTags.h"

UGE_MarkEffect::UGE_MarkEffect()
{
	GrantedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Marked")));
	
	// 지속시간 설정
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(10.0f); // 10초 지속

	// 스태킹 설정 (중복 적용 방지)
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackLimitCount = 1; // 최대 1개만
}
	
	// GrantedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Effect.Mark")));


	// 표식 태그 부여
	// FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(FName("State.Marked"));
	// FGameplayTag EffectTag = FGameplayTag::RequestGameplayTag(FName("Effect.Mark"));
