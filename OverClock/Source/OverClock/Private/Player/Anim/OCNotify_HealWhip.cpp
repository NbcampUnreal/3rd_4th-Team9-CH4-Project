#include "Player/Anim/OCNotify_HealWhip.h"

#include "Weapons/OCWhipMissile.h"
#include "Player/OCFey.h"

void UOCNotify_HealWhip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* Actor = MeshComp->GetAttachParentActor();
	if (!Actor) return;

	if (AOCFey* TheFey = Cast<AOCFey>(Actor))
	{
		// 투사체 발사
		if (TheFey->HasAuthority())
		{
			LaunchMissile(TheFey);
		}
	}
}

void UOCNotify_HealWhip::LaunchMissile(AOCFey* Fey)
{
	if (!Fey) return;
	AOCWhipMissile* Missile = Fey->GetWhipMissileFromPool();
	if (!Missile) return;
	FVector LaunchDirection= GetDirection(Fey, Missile->GetActorLocation());
	if (!LaunchDirection.IsNearlyZero())
	{
		Missile->SetVelocity(LaunchDirection);
	}
}

//조준
FVector UOCNotify_HealWhip::GetDirection(AActor* InAvatar, FVector SpawnLocation)
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
