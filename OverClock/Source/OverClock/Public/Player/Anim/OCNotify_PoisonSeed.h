#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "OCNotify_PoisonSeed.generated.h"

class AOCTheFey;
class AOCPoisonMissile;

UCLASS()
class OVERCLOCK_API UOCNotify_PoisonSeed : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

	void LaunchMissile(AOCTheFey* Fey);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AOCPoisonMissile> MissileActor;
};
