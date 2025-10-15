#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OCGameplayAbility.generated.h"


class UOCAbilitySystemComponent;
class UGameplayEffect;
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;


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

	UPROPERTY(EditDefaultsOnly, Category = "OC|Damage")
	TSubclassOf<UGameplayEffect> DefaultDamageGE;
	
	UFUNCTION(BlueprintPure, Category = "OC|Ability")
	UOCAbilitySystemComponent* GetOCASC() const;
	
	UFUNCTION(BlueprintPure, Category = "OC|Ability")
	ACharacter* GetOCCharacter() const;
	
	UFUNCTION(BlueprintPure, Category = "OC|Ability")
	bool IsServerAuthority() const;

	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask(UAnimMontage* Montage, float PlayRate = 1.f, FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float RootMotionScale = 1.f, float StartTimeSeconds = 0.f, bool bAllowInterruptAfterBlendOut = false) const;

	void ApplyDamageToActor(AActor* TargetActor, float Damage, const FHitResult* OptionalHit = nullptr) const;
	void ApplyDamageToASC(UAbilitySystemComponent* TargetASC, float Damage, const FHitResult* OptionalHit) const;
	void ApplySelfGE_Server(TSubclassOf<UGameplayEffect> EffectClass) const;
	void AddLooseTag(const FGameplayTag& Tag) const;
	void RemoveLooseTag(const FGameplayTag& Tag) const;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanDamageTarget(AActor* Target) const { return true; }
private:
	FGameplayEffectSpecHandle BuildDamageSpec(UAbilitySystemComponent* SourceASC, float Damage, const FHitResult* OptionalHit) const;
};
