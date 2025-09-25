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

   /** (����) GA���� �����Ϳ����� �������̵��ϰ� ���� �� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UOCAnimDataAsset> AnimDataAssetOverride = nullptr;

    // ���� ��Ÿ��(�����Ϳ��¿� Reload�� ���� ��)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reload|Animation")
    TObjectPtr<UAnimMontage> ReloadFallbackMontage = nullptr;

    /** ���� ��Ÿ�� ���� �̸� (AnimBP�� ���� Slot ��尡 �־�� ��) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FName DynamicMontageSlotName = FName(TEXT("UpperBody"));

    // ������ ��� �ӵ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reload|Animation", meta = (ClampMin = "0.1"))
    float PlayRate = 1.0f;

    // ���� ����(������ None)
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
