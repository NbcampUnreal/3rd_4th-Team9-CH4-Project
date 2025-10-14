// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "OCNotify_WildStorm.generated.h"

class AOCWildStormField;
class AOCFey;

UCLASS()
class OVERCLOCK_API UOCNotify_WildStorm : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

	void SpawnAbilityField(AOCFey* Fey);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AOCWildStormField> StormClass;
};
