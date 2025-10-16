#pragma once

#include "CoreMinimal.h"
#include "ItemPool.h"
#include "Player/OCCharacterBase.h"
#include "Actors/OCGemMissile.h"
#include "OCTheFey.generated.h"

class AOCWhipMissile;

UCLASS()
class OVERCLOCK_API AOCTheFey : public AOCCharacterBase
{
	GENERATED_BODY()
public:
	AOCTheFey();

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

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
	
	UFUNCTION()
	void GiveStartAbilities();

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> HealWhipAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> HealGemAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> PoisonSeedAbilityClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> WildStormAbilityClass;

#pragma region Weapon
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

	//Weapon Hidden/Scaling
	UFUNCTION()
	void ScalingWeapon();
	UFUNCTION()
	void HiddenWeapon();

	bool bIsScaling;
	float DeltaSum;
	float CachedAlpha;
	float CachedScale;
#pragma endregion Weapon
};
