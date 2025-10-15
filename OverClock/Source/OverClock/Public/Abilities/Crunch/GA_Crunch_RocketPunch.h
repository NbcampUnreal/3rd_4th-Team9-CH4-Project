#pragma once

#include "CoreMinimal.h"
#include "Abilities/Crunch/GA_CrunchBase.h"
#include "GA_Crunch_RocketPunch.generated.h"

class UAnimMontage;

UCLASS()
class OVERCLOCK_API UGA_Crunch_RocketPunch : public UGA_CrunchBase
{
	GENERATED_BODY()
public:
	UGA_Crunch_RocketPunch();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Dash|Montage")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Dash|Damage")
	float RocketDamage = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Dash|Move")
	float DashDistance = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Dash|Move")
	float DashDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Dash|Move")
	float StepInterval = 0.01f;
	
	UPROPERTY(EditDefaultsOnly, Category="Dash")
	float CapsuleRadius = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="Dash")
	float CapsuleHalfHeight = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Dash|Knockback")
	float KnockbackStrength = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="Dash|Knockback")
	float KnockbackUp = 100.f;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> CachedChar;

	FTimerHandle DashTimer;
	FVector StartLocation;
	FVector DashDirection;
	float MoveDistance = 0.f;
	bool bDashFinished = false;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	void BeginDash_Server();
	void TickDash_Server();
	void FinishDash_Server(bool bHitSomething);
	void ApplyHitToActor_Server(AActor* HitActor, const FVector& From, const FVector& Forward) const;
};
