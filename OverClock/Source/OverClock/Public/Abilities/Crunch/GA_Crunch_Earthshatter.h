#pragma once

#include "CoreMinimal.h"
#include "Abilities/Crunch/GA_CrunchBase.h"
#include "GA_Crunch_Earthshatter.generated.h"

class UAnimMontage;

UCLASS()
class OVERCLOCK_API UGA_Crunch_Earthshatter : public UGA_CrunchBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Ult|Montage")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Ult")
	float Radius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="Ult|Effects")
	TSubclassOf<UGameplayEffect> DamageGE;

	UPROPERTY(EditDefaultsOnly, Category="Ult|Effects")
	TSubclassOf<UGameplayEffect> StunGE;

	UPROPERTY(EditDefaultsOnly, Category="Ult|Gate")
	FGameplayTag UltimateReadyTag;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
