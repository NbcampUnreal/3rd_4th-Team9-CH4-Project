#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_PlayMontage.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class OVERCLOCK_API UGA_PlayMontage : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_PlayMontage();

protected:
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* Task = nullptr;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnTimerExpired();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AbilityMontage;

	FTimerHandle AbilityTimerHandle;
};
