#include "Component/WeaponAmmoComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UWeaponAmmoComponent::UWeaponAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UWeaponAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner(); Owner && Owner->HasAuthority())
	{
		if (CurrentAmmo <= 0) CurrentAmmo = MaxAmmo;
	}
}

void UWeaponAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponAmmoComponent, CurrentAmmo);
}

void UWeaponAmmoComponent::OnRep_CurrentAmmo()
{
	// TODO: HUD °»½Å µî
}

bool UWeaponAmmoComponent::ConsumeAmmo(int32 Amount)
{
	if (!GetOwner()->HasAuthority()) return false;
	if (Amount <= 0) return true;
	if (CurrentAmmo < Amount) return false;

	CurrentAmmo -= Amount;
	return true;
}

void UWeaponAmmoComponent::RefillAmmo()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	CurrentAmmo = MaxAmmo;
}

void UWeaponAmmoComponent::SetCurrentAmmo()
{
	CurrentAmmo = MaxAmmo;
}
