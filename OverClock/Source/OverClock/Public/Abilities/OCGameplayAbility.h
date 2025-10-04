#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OCGameplayAbility.generated.h"


class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EOCAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UCLASS()
class OVERCLOCK_API UOCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UOCGameplayAbility();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OC|Ability")
	EOCAbilityActivationPolicy ActivationPolicy = EOCAbilityActivationPolicy::OnTriggered;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintPure, Category = "OC|Ability")
	UAbilitySystemComponent* GetOCASC() const;
};
