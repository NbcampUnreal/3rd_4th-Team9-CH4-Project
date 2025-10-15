#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAmmoComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OVERCLOCK_API UWeaponAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponAmmoComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo = 6;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo = 0;

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	FORCEINLINE int32 GetAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	FORCEINLINE bool IsAmmoEmpty() const { return CurrentAmmo <= 0; }

	/** 서버에서만 탄 소비(부족하면 false) */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool ConsumeAmmo(int32 Amount = 1);

	/** 서버에서만 풀장전 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void RefillAmmo();

	void SetCurrentAmmo();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(BlueprintCallable)
	void OnRep_CurrentAmmo();
};
