#include "GC/GC_Fire.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

bool UGC_Fire::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	USkeletalMeshComponent* AttachComp = ResolveAttachMesh(Target, Parameters);

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		AttachComp,
		MuzzleSocket,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true);

    
    UGameplayStatics::PlaySoundAtLocation(
        Target,
        FireSound,
        Target->GetActorLocation(),
        VolumeMultiplier,
        PitchMultiplier,
		0.f,
        AttenuationSettings,
        ConcurrencySettings);

	return true;
}

USkeletalMeshComponent* UGC_Fire::ResolveAttachMesh(AActor* Target, const FGameplayCueParameters& Parameters) const
{
    return const_cast<USkeletalMeshComponent*>(CastChecked<const USkeletalMeshComponent>(Parameters.SourceObject.Get()));
}
