// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "GC_Reload.generated.h"

class USoundBase;
class USoundAttenuation;
class USoundConcurrency;
class USkeletalMeshComponent;

UCLASS()
class OVERCLOCK_API UGC_Reload : public UGameplayCueNotify_Burst
{
	GENERATED_BODY()
	
public: 
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    USoundBase* ReloadSound = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Attach")
    FName AttachSocket = FName(TEXT("Muzzle"));; // ¿¹: "S_Mag"

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    float VolumeMultiplier = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    float PitchMultiplier = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    TObjectPtr<USoundAttenuation> AttenuationSettings = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    TObjectPtr<USoundConcurrency> ConcurrencySettings = nullptr;
};
