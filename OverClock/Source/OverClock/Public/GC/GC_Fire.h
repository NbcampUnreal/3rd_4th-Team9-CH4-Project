#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "Sound/SoundBase.h"
#include "Sound/AudioVolume.h"
#include "Kismet/GameplayStatics.h"
#include "GC_Fire.generated.h"

class UNiagaraSystem;
class USkeletalMeshComponent;
 
UCLASS()
class OVERCLOCK_API UGC_Fire : public UGameplayCueNotify_Burst
{
	GENERATED_BODY()
	
public:
	// niagarasystem
	UPROPERTY(EditDefaultsOnly, Category = "MuzzleFX")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MuzzleFX")
	FName MuzzleSocket = FName(TEXT("Muzzle"));

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> FireSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundAttenuation> AttenuationSettings = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundConcurrency> ConcurrencySettings = nullptr;

protected:
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

private:
	USkeletalMeshComponent* ResolveAttachMesh(AActor* Target, const FGameplayCueParameters& Parameters) const;
};
