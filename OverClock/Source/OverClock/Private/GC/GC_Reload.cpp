#include "GC/GC_Reload.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

bool UGC_Reload::OnExecute_Implementation(
    AActor* Target, const FGameplayCueParameters& Parameters) const
{

    UGameplayStatics::PlaySoundAtLocation(
        Target,
        ReloadSound,
        Target->GetActorLocation(),
        VolumeMultiplier,
        PitchMultiplier,
        0.f,
        AttenuationSettings,
        ConcurrencySettings);

    return true;
}