#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_DeadlyBullet.generated.h"

class UGE_MarkEffect;
class AOCRBMissile;

UCLASS()
class OVERCLOCK_API UGA_DeadlyBullet : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_DeadlyBullet();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, bool bWasCancelled) override;

	// 타겟 탐지 범위
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 1500.0f;

	// 유도탄 발사체 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AOCRBMissile> MissileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mark")
	float MarkDuration = 10.0f;
	
private:
	// 가장 가까운 적 찾기
	AActor* FindNearestEnemy();
    
	// 유도탄 발사
	void LaunchHomingProjectile(AActor* Target);
};
