#pragma once

#include "CoreMinimal.h"
#include "Abilities/OCGameplayAbility.h"
#include "GA_CrunchBase.generated.h"

class UGameplayEffect;
class UOCAbilitySystemComponent;
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

UCLASS(Abstract)
class OVERCLOCK_API UGA_CrunchBase : public UOCGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CrunchBase();

protected:
	void BoxTraceAndApplyToTargets_Server(const FVector& Start, const FVector& End, const FVector& HalfExtent, TSubclassOf<UGameplayEffect> GE, bool bStopAtFirstHit = true) const;
	void SphereTraceAndApplyToTargets_Server(const FVector& Center, float Radius, TSubclassOf<UGameplayEffect> DamageGE, TSubclassOf<UGameplayEffect> CcGE) const;
	void CapsuleSweepAndApplyToTargets_Server(const FVector& Start, const FVector& End, float Radius, float HalfHeight, TSubclassOf<UGameplayEffect> GE) const;
	
	UOCAbilitySystemComponent* GetTargetOCASC(const AActor* Target) const;
};
