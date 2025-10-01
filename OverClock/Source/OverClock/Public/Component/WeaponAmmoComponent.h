// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAmmoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERCLOCK_API UWeaponAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponAmmoComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	FORCEINLINE int32 GetAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	FORCEINLINE bool IsAmmoEmpty() const { return CurrentAmmo <= 0; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool ConsumeAmmo(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void RefillAmmo();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentAmmo();
};
