// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_DeathBullet.generated.h"

class UAnimSequenceBase;
class UAnimMontage;
class UOCAnimDataAsset;
class UAbilityTask_WaitInputPress;
class UAbilityTask_WaitDelay;
class UGE_RangedAttackCooldown;

/**
 * E 를 누르면 과장된 장전(RMB Reload Med)을 재생하고,
 * InputWindowSec 내에 좌클릭이 들어오면 C->C->B 세 발을 빠르게 재생.
 * 입력이 없으면 취소되지만 쿨타임은 적용.
 * 실행 동안 State.ActionLock 으로 다른 스킬 차단.
 */

UCLASS()
class OVERCLOCK_API UGA_DeathBullet : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_DeathBullet();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 크게 장전하는 모션
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Anim")
	TObjectPtr<UAnimSequenceBase> ReloadAnim;

	// C->C->B 할때 C애니메이션
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Anim")
	TObjectPtr<UAnimSequenceBase> FireCAnim;

	// C->C->B 할때 B애니메이션
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Anim")
	TObjectPtr<UAnimSequenceBase> FireBAnim;

	// 동적 몽타주 슬롯명
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Anim")
	FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

	// 몽타주 재생 배속
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Anim")
	float PlayRate = 1.0f;
	// 연사 간격
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Timing")
	float ShotIntervalSec = 0.06f;

	// 좌클릭 입력 시간
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Timing")
	float InputWindowSec = 3.0f;

	// 스킬 쿹타임 : 실제 적용은 GE사용할 예정
	UPROPERTY(EditAnywhere, Category = "DeathBullet|Cooldown")
	float CooldownSeconds = 6.0f;

	// 쿨타임 GE
	UPROPERTY(EditDefaultsOnly, Category = "DeathBullet|Cooldown")
	TSubclassOf<UGameplayEffect> CooldownGE;

	// 내부 상태
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputPress> WaitPressTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> WaitTimeoutTask;

	bool bTriggered = false;

protected:
	// 유틸
	UAnimMontage* MakeDynamicMontage(const UAnimSequenceBase* Seq) const;
	void PlayAnim_ServerMulticast(const UAnimSequenceBase* Seq) const;
	void PlaymuzzleCue(const FGameplayAbilityActorInfo* ActorInfo) const;

	UFUNCTION()
	void OnPressedCallback(float TimeWaited);

	void ApplyCooldownWithTags(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		float DurationSec) const;

};
