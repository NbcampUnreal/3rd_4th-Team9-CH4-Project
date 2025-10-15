// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemPool.h"
#include "Player/OCCharacterBase_V2.h"
#include "Weapons/OCWhipMissile.h"
#include "Weapons/OCGemMissile.h"

#include "OCFey.generated.h"

class UGameplayAbility;

UCLASS()
class OVERCLOCK_API AOCFey : public AOCCharacterBase_V2
{
	GENERATED_BODY()
public:
	AOCFey();
	virtual ~AOCFey() override;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Weapon)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon)
	TObjectPtr<USceneComponent> WeaponRoot;

	UFUNCTION()
	FVector GetMuzzleLocation() const
	{
		return WeaponRoot->GetComponentLocation();
	}

	UFUNCTION()
	FRotator GetMuzzleRotation() const
	{
		return WeaponRoot->GetComponentRotation();
	}

	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> HealWhipAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> HealGemAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> PoisonSeedAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> WildStormAbilityClass;

#pragma region Weapon
	
	void OnHiddenTagChanged(const FGameplayTag GameplayTag, int32 NewCount);
	void OnScalingTagChanged(const FGameplayTag GameplayTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> ScalingEffect;
	
	/*Whip Pool*/
	TUniquePtr<TItemPool<AOCWhipMissile>> WhipPool;

	UPROPERTY(EditAnywhere,Category=Weapon)
	TSubclassOf<AOCWhipMissile> WhipPoolClass;
	UPROPERTY(EditAnywhere,Category=Weapon)
	int32 WhipPoolSize;
	
	UFUNCTION(Category="Weapon")
	AOCWhipMissile* GetWhipMissileFromPool();

	/*Gem Pool*/
	TUniquePtr<TItemPool<AOCGemMissile>> GemPool;

	UPROPERTY(EditAnywhere,Category=Weapon)
	TSubclassOf<AOCGemMissile> GemPoolClass;
	UPROPERTY(EditAnywhere,Category=Weapon)
	int32 GemPoolSize;
	
	UFUNCTION(Category="Weapon")
	AOCGemMissile* GetGemMissileFromPool();

	bool bIsScaling;
	float DeltaSum;
	float CachedAlpha;
	float CachedScale;
#pragma endregion Weapon	
};
