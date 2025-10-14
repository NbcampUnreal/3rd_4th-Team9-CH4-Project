#pragma once

#include "CoreMinimal.h"
#include "Player/OCCharacterBase_V2.h"
#include "Component/WeaponAmmoComponent.h"
#include "OCTwinBlast.generated.h"

UCLASS()
class OVERCLOCK_API AOCTwinBlast : public AOCCharacterBase_V2
{
	GENERATED_BODY()
	
public:
	AOCTwinBlast();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponAmmoComponent* WeaponAmmoComp = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_RequestReloadRefill();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
