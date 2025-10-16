// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/TwinBlast/GA_SharedCooldownBase.h"
#include "GA_DeathBullet.generated.h"

class UAnimSequenceBase;
class UAnimMontage;
class UOCAnimDataAsset;

/**
 * E �� ������ ����� ����(RMB Reload Med)�� ����ϰ�,
 * InputWindowSec ���� ��Ŭ���� ������ C->C->B �� ���� ������ ���.
 * �Է��� ������ ��ҵ����� ��Ÿ���� ����.
 * ���� ���� State.ActionLock ���� �ٸ� ��ų ����.
 */

UCLASS()
class OVERCLOCK_API UGA_DeathBullet : public UGA_SharedCooldownBase
{
	GENERATED_BODY()

public:
	UGA_DeathBullet();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DeathBullet|Anim")
	FName DynamicMontageSlotName = TEXT("UpperBody");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DeathBullet|Anim")
	float PlayRate = 1.f;

	UPROPERTY(EditAnywhere, Category = "DeathBullet|Config", meta = (ClampMin = "1"))
	int32 EmpoweredShots = 3;

	UPROPERTY(EditAnywhere, Category = "DeathBullet|Config", meta = (ClampMin = "0.0"))
	float WindowDuration = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "DeathBullet|Tags", meta = (Categories = "State"))
	FGameplayTag WindowTag;            // ex) State.DeathBullet.Active

	UPROPERTY(EditDefaultsOnly, Category = "DeathBullet|Tags", meta = (Categories = "Data"))
	FGameplayTag Data_DurationTag;

	UPROPERTY(EditDefaultsOnly, Category = "DeathBullet|GE")
	TSubclassOf<UGameplayEffect> DeathBulletWindowGE;


	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION() 
	void OnAnyEffectRemoved(const FActiveGameplayEffect& Removed);

	FActiveGameplayEffectHandle WindowHandle;
};
