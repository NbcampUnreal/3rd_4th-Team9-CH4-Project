#include "GC/GC_Reload.h"
#include "Data/ReloadSoundDataAsset.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

bool UGC_Reload::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	const FGameplayTag Tag = Parameters.OriginalTag;

	USoundBase* ReloadSFX = SoundSet->Sounds.FindRef(Tag);

	check(ReloadSFX);

	UGameplayStatics::PlaySoundAtLocation(
		Target,
		ReloadSFX,
		Target->GetActorLocation(),
		VolumeMultiplier,
		PitchMultiplier,
		/*StartTime=*/0.f,
		AttenuationSettings,
		ConcurrencySettings
	);

	UE_LOG(LogTemp, Warning, TEXT("GC_Reload running on: %s  (SoundSet=%s)"), *GetNameSafe(this), *GetNameSafe(SoundSet));

	return true;
}