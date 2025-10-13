// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ReloadSoundDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class OVERCLOCK_API UReloadSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TMap<FGameplayTag, TObjectPtr<USoundBase>> Sounds;
};
