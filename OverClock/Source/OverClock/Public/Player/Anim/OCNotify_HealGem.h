#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "OCNotify_HealGem.generated.h"

class AOCTheFey;
class AOCGemMissile;

UCLASS()
class OVERCLOCK_API UOCNotify_HealGem : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

	void LaunchMissile(AOCTheFey* Fey);

	FVector GetDirection(AActor* InAvatar, FVector SpawnLocation);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AOCGemMissile> MissileActor;	
};
