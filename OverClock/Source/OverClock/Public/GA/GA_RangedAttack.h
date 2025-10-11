#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_RangedAttack.generated.h"

class UOCAnimDataAsset;
class UAnimMontage;
class UGameplayEffect;

/**
 * 좌클릭 사격: 1발 = 1애니메이션 (재생 중 재발동 금지는 '쿨타임 GE'로 처리)
 * - AnimDataAsset의 PrimaryAttack 사용(시퀀스면 동적 몽타주로 변환)
 * - 재생만 하고 즉시 EndAbility (상태 태그 잔류 없음)
 * - 쿨타임은 몽타주 길이/PlayRate 만큼 런타임에서 Duration을 오버라이드하여 부여
 */
UCLASS()
class OVERCLOCK_API UGA_RangedAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_RangedAttack();

protected:
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags,
        const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

private:
    UFUNCTION() 
    void OnMontageCompleted();
    UFUNCTION() 
    void OnMontageInterrupted();

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    float PlayRate = 1.f;

    UPROPERTY(EditAnywhere, Category = "Anim")
    TObjectPtr<UAnimSequenceBase> EmpoweredShotAnim = nullptr;

    
    //line trace
protected:
    void PerformCameraTraceAndFire(const FGameplayAbilityActorInfo* ActorInfo);

    UPROPERTY(BlueprintReadOnly, Category = "Ability|Trace")
    FVector FinalTargetLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability|Trace")
    bool bHitTargetActor = false;
};
