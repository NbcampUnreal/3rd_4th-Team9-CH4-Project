#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_Reload.generated.h"

class UAnimMontage;
class UAnimSequenceBase;
class UGameplayEffect;
class UOCAnimDataAsset;

/**
 * Reload 능력 (InstancedPerActor)
 * - 애니 재생(동적/정적 몽타주 지원)
 * - 리로드 사운드/이펙트는 GameplayCue 쪽에서 처리
 * - 쿨타임 GE에 DynamicGrantedTags로 태그를 수동 부여(UE5.5 대응)
 */
UCLASS()
class UReload : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UReload();

	// 활성화 가능 여부 (애니/쿨타임 확인)
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	// 실제 실행
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	// 리로드용 몽타주 결정(시퀀스면 동적 몽타주로 래핑)
	UAnimMontage* ResolveReloadMontage(const FGameplayAbilityActorInfo* ActorInfo) const;

	// 쿨타임 GE 스펙 생성(레벨 적용)
	FGameplayEffectSpecHandle MakeOutgoingCooldownSpec(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo) const;

	/** 캐릭터별 애니 데이터를 제공하는 데이터 에셋(없으면 캐릭터가 들고있는 에셋 사용) */
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Animation")
	TObjectPtr<const UOCAnimDataAsset> AnimDataAssetOverride = nullptr;

	/** 최종 폴백 몽타주(데이터 에셋이 없거나 Reload 미설정 시 사용) */
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Animation")
	TObjectPtr<UAnimMontage> ReloadFallbackMontage = nullptr;

	/** 동적 몽타주 생성 시 사용할 슬롯명(AnimBP 슬롯과 일치 필요) */
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Animation")
	FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

	/** 리로드 애니 속도 배수 */
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Animation")
	float PlayRate = 1.0f;

	/** 재생할 섹션명(없으면 전체) */
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Animation")
	FName SectionName = NAME_None;

	/** 리로드 쿨타임용 GE 클래스(HasDuration) */
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Cooldown")
	TSubclassOf<UGameplayEffect> CooldownGE;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FScalableFloat CooldownSec = 0.0f;
};