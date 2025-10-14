#include "Player/OCTwinBlast.h"

AOCTwinBlast::AOCTwinBlast()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

void AOCTwinBlast::BeginPlay()
{
	Super::BeginPlay();

	WeaponAmmoComp = FindComponentByClass<UWeaponAmmoComponent>();
	ensureAlwaysMsgf(WeaponAmmoComp, TEXT("WeaponAmmo comp missing!"));
}

void AOCTwinBlast::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AOCTwinBlast::Server_RequestReloadRefill_Implementation()
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