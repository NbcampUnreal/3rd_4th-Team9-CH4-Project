#include "Player/Anim/OCNotify_WildStorm.h"

#include "Player/OCTheFey.h"
#include "Weapons/OCWildStormField.h"

void UOCNotify_WildStorm::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
			SpawnAbilityField(TheFey);
		}
	}
}

void UOCNotify_WildStorm::SpawnAbilityField(AOCTheFey* Fey)
{
	if (!StormClass) return;
	GetWorld()->SpawnActor<AOCWildStormField>(
		StormClass,
		Fey->GetActorLocation(),
		FRotator::ZeroRotator
	);
}
