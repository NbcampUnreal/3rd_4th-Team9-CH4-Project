#include "Component/WeaponAmmoComponent.h"

#include "Gameframework/Actor.h"
#include "Engine/World.h"
#include <Net/UnrealNetwork.h>

UWeaponAmmoComponent::UWeaponAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UWeaponAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (CurrentAmmo <= 0)
		{
			CurrentAmmo = MaxAmmo;
		}
	}
}

bool UWeaponAmmoComponent::ConsumeAmmo(int32 Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;

	if (Amount <= 0) return true;

	if (CurrentAmmo < Amount) return false;

	const int32 Before = CurrentAmmo;

	CurrentAmmo -= Amount;

	UE_LOG(LogTemp, Log, TEXT("[AmmoComp] Consume %d -> %d -> %d"), Amount, Before, CurrentAmmo);

	return true;
}

void UWeaponAmmoComponent::RefillAmmo()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	const int32 Before = CurrentAmmo;

	CurrentAmmo = MaxAmmo;

	UE_LOG(LogTemp, Log, TEXT("[AmmoComp] Refill -> %d -> %d"), Before, CurrentAmmo);
}


void UWeaponAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponAmmoComponent, CurrentAmmo);
}

void UWeaponAmmoComponent::OnRep_CurrentAmmo()
{
	UE_LOG(LogTemp, Log, TEXT("[AmmoComp] OnRep: %d / %d"), CurrentAmmo, MaxAmmo);
}

