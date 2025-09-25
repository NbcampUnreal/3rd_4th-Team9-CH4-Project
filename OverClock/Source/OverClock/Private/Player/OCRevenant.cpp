// OCRevenant.cpp
#include "Player/OCRevenant.h"
#include "Player/OCPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include <Net/UnrealNetwork.h>

AOCRevenant::AOCRevenant()
{
	bReplicates = true;
}

void AOCRevenant::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		if (CurrentAmmo <= 0) CurrentAmmo = MaxAmmo;
		UE_LOG(LogTemp, Log, TEXT("[Ammo] BeginPlay: %d / %d"), CurrentAmmo, MaxAmmo);
	}
}

void AOCRevenant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOCRevenant, CurrentAmmo);
}

TSubclassOf<UGameplayAbility> AOCRevenant::GetAbilityClassByTag(FGameplayTag AbilityTag) const
{
    if (!AbilityTag.IsValid()) return nullptr;
    if (const TSubclassOf<UGameplayAbility>* Found = AbilityMapByTag.Find(AbilityTag))
    {
        return *Found;
    }
    return nullptr;
}

void AOCRevenant::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
		if (HasAuthority()) GiveRevenantStartupAbilities();
	}
}

void AOCRevenant::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
	}
}

void AOCRevenant::GiveRevenantStartupAbilities()
{
	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
	if (!PS || !HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
	{
		if (PeacekeeperAbilityClass && !ASC->FindAbilitySpecFromClass(PeacekeeperAbilityClass))
		{
			ASC->GiveAbility(FGameplayAbilitySpec(PeacekeeperAbilityClass, 1, 0, this));
		}
	}
}

// ==== [ADD] ammo helpers =====================================================
void AOCRevenant::OnRep_CurrentAmmo()
{
	UE_LOG(LogTemp, Log, TEXT("[Ammo] OnRep: %d / %d"), CurrentAmmo, MaxAmmo);
}

bool AOCRevenant::ConsumeAmmo(int32 Amount /*=1*/)
{
	if (!HasAuthority()) return false;         // 서버에서만 변경
	if (Amount <= 0) return true;
	if (CurrentAmmo < Amount) return false;

	const int32 Before = CurrentAmmo;
	CurrentAmmo -= Amount;
	UE_LOG(LogTemp, Log, TEXT("[Ammo] Consume %d -> %d -> %d"), Amount, Before, CurrentAmmo);
	return true;
}

void AOCRevenant::RefillAmmo()
{
	if (!HasAuthority()) return;
	const int32 Before = CurrentAmmo;
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogTemp, Log, TEXT("[Ammo] Refill -> %d -> %d"), Before, CurrentAmmo);
}
