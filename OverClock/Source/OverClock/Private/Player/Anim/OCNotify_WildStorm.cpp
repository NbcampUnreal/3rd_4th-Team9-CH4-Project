#include "Player/Anim/OCNotify_WildStorm.h"

#include "Player/OCFey.h"
#include "Weapons/OCWildStormField.h"

void UOCNotify_WildStorm::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AActor* Actor = MeshComp->GetAttachParentActor();
	if (!Actor) return;

	if (AOCFey* TheFey = Cast<AOCFey>(Actor))
	{
		// 투사체 발사
		if (TheFey->HasAuthority())
		{
			SpawnAbilityField(TheFey);
		}
	}
}

void UOCNotify_WildStorm::SpawnAbilityField(AOCFey* Fey)
{
	if (!StormClass || !Fey) return;
	UWorld* World = Fey->GetWorld();
	if (!IsValid(World)) return;
	World->SpawnActor<AOCWildStormField>(
		StormClass,
		Fey->GetActorLocation(),
		FRotator::ZeroRotator
	);
}
