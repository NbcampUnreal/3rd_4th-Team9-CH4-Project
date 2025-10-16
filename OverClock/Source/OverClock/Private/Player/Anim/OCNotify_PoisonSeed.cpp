#include "Player/Anim/OCNotify_PoisonSeed.h"

#include "Actors/OCPoisonMissile.h"
#include "Player/OCFey.h"

void UOCNotify_PoisonSeed::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	AActor* Actor = MeshComp->GetAttachParentActor();
	if (!Actor) return;

	if (AOCFey* TheFey = Cast<AOCFey>(Actor))
	{
		LaunchMissile(TheFey);
	}
}

void UOCNotify_PoisonSeed::LaunchMissile(AOCFey* Fey)
{
	if (!IsValid(Fey) || !MissileActor) return;

	UWorld* World = Fey->GetWorld();
	if (!IsValid(World)) return;
	
	FActorSpawnParameters Params;
	Params.Owner = Fey;

	const FVector SpawnLoc = Fey->GetMuzzleLocation();
	const FRotator SpawnRot = Fey->GetMuzzleRotation();

	AOCPoisonMissile* Missile = World->SpawnActor<AOCPoisonMissile>(
		MissileActor,
		SpawnLoc,
		SpawnRot,
		Params
	);
	if (Missile)
	{
		Missile->SetVelocity(Fey->GetActorForwardVector());
	}
}
