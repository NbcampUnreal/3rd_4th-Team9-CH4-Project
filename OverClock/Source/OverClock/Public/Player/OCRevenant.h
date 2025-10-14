// OCRevenant.h
#pragma once

#include "CoreMinimal.h"
#include "Player/OCCharacterBase.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "OCRevenant.generated.h"

class UWeaponAmmoComponent;

UCLASS()
class OVERCLOCK_API AOCRevenant : public AOCCharacterBase
{
	GENERATED_BODY()
public:
	AOCRevenant();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponAmmoComponent* WeaponAmmoComp = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_RequestReloadRefill();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
