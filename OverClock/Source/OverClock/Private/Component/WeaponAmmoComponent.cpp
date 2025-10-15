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
	static const FName FuncName(TEXT("OnAmmoChanged"));
	if (UFunction* SelfFn = FindFunction(FuncName))
	{
		struct { int32 Current; int32 Max; } Params{ CurrentAmmo, MaxAmmo };
		ProcessEvent(SelfFn, &Params);
	}

	if (AActor* Owner = GetOwner())
	{
		if (UFunction* OwnerFn = Owner->FindFunction(FuncName))
		{
			struct { int32 Current; int32 Max; } Params{ CurrentAmmo, MaxAmmo };
			Owner->ProcessEvent(OwnerFn, &Params);
		}
	}
}

bool UWeaponAmmoComponent::ConsumeAmmo(int32 Amount)
{
	if (!GetOwner()->HasAuthority()) return false;
	if (Amount <= 0) return true;
	if (CurrentAmmo < Amount) return false;

	CurrentAmmo -= Amount;
	OnRep_CurrentAmmo();
	return true;
}

void UWeaponAmmoComponent::RefillAmmo()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	CurrentAmmo = MaxAmmo;
	OnRep_CurrentAmmo();
}

void UWeaponAmmoComponent::SetCurrentAmmo()
{
	CurrentAmmo = MaxAmmo;
	OnRep_CurrentAmmo();
}
