#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_HealWhip.generated.h"

class AOCFeyMissile;

UCLASS()
class OVERCLOCK_API UGA_HealWhip : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_HealWhip();

protected:
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<AOCFeyMissile> MissileActor;

	
	
};
