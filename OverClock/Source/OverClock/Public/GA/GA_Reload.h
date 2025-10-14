#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_Reload.generated.h"

class UAnimMontage;
class UAnimSequenceBase;
class UGameplayEffect;
class UOCAnimDataAsset;

/**
 * Reload 능력 (InstancedPerActor)
 * - 애니 재생(동적/정적 몽타주 지원)
 * - 리로드 사운드/이펙트는 GameplayCue 쪽에서 처리
 * - 쿨타임 GE에 DynamicGrantedTags로 태그를 수동 부여(UE5.5 대응)
 */
UCLASS()
class OVERCLOCK_API UGA_Reload : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Reload();

protected:
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

    UPROPERTY(EditDefaultsOnly, Category = "Cue")
    TMap<FGameplayTag, FGameplayTag> ReloadCueByType;

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    float PlayRate = 1.f;
};