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
// 		PS->InitASCForAvatar(this);
// 		if (HasAuthority()) GiveRevenantStartupAbilities();
// 	}
// }

// void AOCRevenant::OnRep_PlayerState()
// {
// 	Super::OnRep_PlayerState();
// 	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
// 	{
// 		PS->InitASCForAvatar(this);
// 	}
// }

// void AOCRevenant::GiveRevenantStartupAbilities()
// {
// 	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
// 	if (!PS || !HasAuthority()) return;

// 	if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
// 	{
// 		if (DeadlyBulletAbilityClass && !ASC->FindAbilitySpecFromClass(DeadlyBulletAbilityClass))
// 		{
// 			ASC->GiveAbility(FGameplayAbilitySpec(DeadlyBulletAbilityClass, 1, 0, this));
// 		}
		
// 		if (PeacekeeperAbilityClass && !ASC->FindAbilitySpecFromClass(PeacekeeperAbilityClass))
		const int32 Before = Ammo->CurrentAmmo;
		if (Before < Ammo->MaxAmmo)
		{
			Ammo->RefillAmmo();
		}
		UE_LOG(LogTemp, Log, TEXT("[ReloadFix] Server refill %d -> %d / Max=%d"),
			Before, Ammo->CurrentAmmo, Ammo->MaxAmmo);
	}
}

// ==== [ADD] ammo helpers =====================================================
void AOCRevenant::OnRep_CurrentAmmo()
{
	//UE_LOG(LogTemp, Log, TEXT("[Ammo] OnRep: %d / %d"), CurrentAmmo, MaxAmmo);
}

bool AOCRevenant::ConsumeAmmo(int32 Amount /*=1*/)
{
	// if (!HasAuthority()) return false;         // ���������� ����
	// if (Amount <= 0) return true;
	// if (CurrentAmmo < Amount) return false;

	// const int32 Before = CurrentAmmo;
	// CurrentAmmo -= Amount;
	// UE_LOG(LogTemp, Log, TEXT("[Ammo] Consume %d -> %d -> %d"), Amount, Before, CurrentAmmo);
	// return true;
}

void AOCRevenant::RefillAmmo()
{
	// if (!HasAuthority()) return;
	// const int32 Before = CurrentAmmo;
	// CurrentAmmo = MaxAmmo;
	// UE_LOG(LogTemp, Log, TEXT("[Ammo] Refill -> %d -> %d"), Before, CurrentAmmo);
}
