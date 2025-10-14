#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "OCNotify_HealWhip.generated.h"

class AOCFey;
class AOCWhipMissile;

UCLASS()
class OVERCLOCK_API UOCNotify_HealWhip : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

	void LaunchMissile(AOCFey* Fey);

	FVector GetDirection(AActor* InAvatar, FVector SpawnLocation);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AOCWhipMissile> MissileActor;

};
