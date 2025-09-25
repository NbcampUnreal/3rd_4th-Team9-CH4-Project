#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_Peacekeeper.generated.h"

class UOCAnimDataAsset;
class UAnimMontage;
class UGameplayEffect;

/**
 * 좌클릭 사격: 1발 = 1애니메이션 (재생 중 재발동 금지는 '쿨타임 GE'로 처리)
 * - AnimDataAsset의 PrimaryAttack 사용(시퀀스면 동적 몽타주로 변환)
 * - 재생만 하고 즉시 EndAbility (상태 태그 잔류 없음)
 * - 쿨타임은 몽타주 길이/PlayRate 만큼 런타임에서 Duration을 오버라이드하여 부여
 */
UCLASS()
class OVERCLOCK_API UGA_Peacekeeper : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Peacekeeper();

	/** (선택) GA에서 데이터에셋을 오버라이드하고 싶을 때 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UOCAnimDataAsset> AnimDataAssetOverride = nullptr;

	/** 폴백 몽타주(데이터에셋이 비었을 때 사용) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FallbackMontage = nullptr;

	/** 동적 몽타주 슬롯 이름 (AnimBP에 같은 Slot 노드가 있어야 함) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

	/** 재생 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = "0.1"))
	float PlayRate = 1.0f;

	/** (선택) 섹션으로 시작할 때 이름(없으면 None) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName SectionName = NAME_None;

	/** 쿨타임 GE (HasDuration) — Duration은 런타임에서 애니 길이로 설정됨 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownGE;

	// GAS overrides
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** 데이터에셋 PrimaryAttack → UAnimMontage(필요 시 동적 생성) */
	UAnimMontage* ResolveMontage(const FGameplayAbilityActorInfo* ActorInfo) const;

	/** 쿨타임 스펙 생성 (Duration은 호출부에서 SetDuration) */
	FGameplayEffectSpecHandle MakeOutgoingCooldownSpec(
		const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const;
};
