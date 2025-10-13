#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_DeadlyBullet.generated.h"

class UGE_MarkEffect;
class AOCRBMissile;
class UAnimMontage;

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

	void OnMontageCompleted();

	void OnMontageCancelled();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AOCRBMissile> MissileClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Mark")
	UAnimMontage* MarkMontage;
	
private:
	AActor* FindNearestEnemy();
    
	void LaunchHomingProjectile(AActor* Target);
};
