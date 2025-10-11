// OCRevenant.cpp
#include "Player/OCRevenant.h"
#include "Component/WeaponAmmoComponent.h"
#include "Net/UnrealNetwork.h"

AOCRevenant::AOCRevenant()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

void AOCRevenant::BeginPlay()
{
	Super::BeginPlay();

	WeaponAmmoComp = FindComponentByClass<UWeaponAmmoComponent>();
	ensureAlwaysMsgf(WeaponAmmoComp, TEXT("WeaponAmmo comp missing!"));
}

void AOCRevenant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AOCRevenant::Server_RequestReloadRefill_Implementation()
{
	if (UWeaponAmmoComponent* Ammo = WeaponAmmoComp)
	{
		const int32 Before = Ammo->CurrentAmmo;
		if (Before < Ammo->MaxAmmo)
		{
			Ammo->RefillAmmo();
		}
		UE_LOG(LogTemp, Log, TEXT("[ReloadFix] Server refill %d -> %d / Max=%d"),
			Before, Ammo->CurrentAmmo, Ammo->MaxAmmo);
	}
}
