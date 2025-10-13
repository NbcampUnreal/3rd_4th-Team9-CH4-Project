#include "Player/Anim/OCNotify_HealGem.h"

#include "Player/OCTheFey.h"

void UOCNotify_HealGem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AActor* Actor = MeshComp->GetAttachParentActor();
	if (!Actor) return;

	if (AOCTheFey* TheFey = Cast<AOCTheFey>(Actor))
	{
		// 투사체 발사
		if (TheFey->HasAuthority())
		{
			LaunchMissile(TheFey);
		}
	}
}

void UOCNotify_HealGem::LaunchMissile(AOCTheFey* Fey)
{
	if (!Fey) return;
	AOCGemMissile* Missile = Fey->GetGemMissileFromPool();
	if (!Missile) return;
	FVector LaunchDirection= GetDirection(Fey, Missile->GetActorLocation());
	if (!LaunchDirection.IsNearlyZero())
	{
		Missile->SetVelocity(LaunchDirection);
	}
}

FVector UOCNotify_HealGem::GetDirection(AActor* InAvatar, FVector SpawnLocation)
{
	if (!InAvatar) return FVector::ZeroVector;

	APlayerController* PC = Cast<APlayerController>(InAvatar->GetInstigatorController());
	if (!PC) return FVector::ZeroVector;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector TargetPoint = CamLoc + CamRot.Vector() * 10000.f; // 100m 앞
	FVector LaunchDirection = (TargetPoint - SpawnLocation).GetSafeNormal();

	return LaunchDirection;
}
