// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RevReload.generated.h"

class UOCAnimDataAsset;
class UAnimMontage;
class UAnimSequenceBase;

UCLASS()
class OVERCLOCK_API URevReload : public UGameplayAbility
{
	GENERATED_BODY()

public:
    URevReload();

   /** (선택) GA에서 데이터에셋을 오버라이드하고 싶을 때 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UOCAnimDataAsset> AnimDataAssetOverride = nullptr;

    // 폴백 몽타주(데이터에셋에 Reload가 없을 때)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reload|Animation")
    TObjectPtr<UAnimMontage> ReloadFallbackMontage = nullptr;

    /** 동적 몽타주 슬롯 이름 (AnimBP에 같은 Slot 노드가 있어야 함) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

    // 재장전 재생 속도
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reload|Animation", meta = (ClampMin = "0.1"))
    float PlayRate = 1.0f;

    // 시작 섹션(없으면 None)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reload|Animation")
    FName SectionName = NAME_None;

    UPROPERTY(EditAnywhere, Category = "GE")
    TSubclassOf<class UGameplayEffect> CooldownGE;

protected:
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags,
        const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    // Helpers
    UAnimMontage* ResolveReloadMontage(const FGameplayAbilityActorInfo* ActorInfo) const;
    FGameplayEffectSpecHandle MakeOutgoingCooldownSpec(
        const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const;
};
