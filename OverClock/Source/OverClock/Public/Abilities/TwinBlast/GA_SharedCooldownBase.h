// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "GA_SharedCooldownBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class OVERCLOCK_API UGA_SharedCooldownBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SharedCooldownBase();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> SharedCooldownGE;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown|Tags", meta = (AllowPrivateAccess="true"))
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (Clampmin="0", AllowPrivateAccess="true"))
	FScalableFloat CooldownSeconds;

    UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (AllowPrivateAccess = "true"))
    FGameplayTag SetByCaller_DurationKey;

    bool BuildCooldownSpec(
        FGameplayEffectSpecHandle& OutSpecHandle,
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo
    ) const;

    // UGameplayAbility API
    virtual void ApplyCooldown(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) const override;

    virtual const FGameplayTagContainer* GetCooldownTags() const override;

    virtual UGameplayEffect* GetCooldownGameplayEffect() const override;

    mutable FGameplayTagContainer CooldownTagsCache;

public:
    /** 런타임에서 동적으로 쿨타임 초 갱신이 필요할 때 편의 함수 */
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void SetCooldownSeconds(float InSeconds);

    /** 런타임에서 동적으로 쿨다운 태그 변경이 필요할 때 편의 함수 */
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void SetCooldownTag(FGameplayTag InTag);

    /** 에디터/코드에서 공용 GE 지정 */
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void SetSharedCooldownGE(TSubclassOf<class UGameplayEffect> InGE);

    /** SetByCaller 키 지정(기본값: Data.Cooldown.Duration) */
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void SetSetByCallerDurationKey(FGameplayTag InKey);
};
