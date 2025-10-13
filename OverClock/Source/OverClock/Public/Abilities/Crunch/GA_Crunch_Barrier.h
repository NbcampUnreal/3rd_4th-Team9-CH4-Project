#pragma once

#include "CoreMinimal.h"
#include "Abilities/Crunch/GA_CrunchBase.h"
#include "GA_Crunch_Barrier.generated.h"


class UAnimMontage;

UCLASS()
class OVERCLOCK_API UGA_Crunc_Barrier : public UGA_CrunchBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Barrier|Montage")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Barrier|Spawn")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Barrier|Spawn")
	FName ThrowSocket = "RightHandSocket";

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageInterrupted();
};
