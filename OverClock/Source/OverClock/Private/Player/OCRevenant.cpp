// OCRevenant.cpp
#include "Player/OCRevenant.h"
#include "Player/OCPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

AOCRevenant::AOCRevenant()
{
	bReplicates = true;
	SetReplicateMovement(true);
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

void AOCRevenant::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, float InPlayRate, FName InSection)
{
	if (!Montage) return;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
	{
		if (!Anim->Montage_IsPlaying(Montage))
		{
			const float Ret = Anim->Montage_Play(Montage, InPlayRate);
			if (Ret > 0.f && !InSection.IsNone() && Montage->IsValidSectionName(InSection))
			{
				Anim->Montage_JumpToSection(InSection, Montage);
			}
		}
	}
}

void AOCRevenant::Multicast_PlayReloadByAsset_Implementation(UAnimSequenceBase* ReloadSource, FName SlotName, float InPlayRate, FName Section)
{
	if (!ReloadSource) return;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
	{
		// 각 클라이언트가 "동일한" 동적 몽타주를 로컬에서 생성
		UAnimMontage* Dyn = UAnimMontage::CreateSlotAnimationAsDynamicMontage(
			ReloadSource, SlotName,
			/*BlendIn*/0.2f, /*BlendOut*/0.2f,
			/*PlayRate*/FMath::Max(InPlayRate, 0.01f),
			/*LoopCount*/1, /*BlendOutTriggerTime*/0.f, /*InTimeToStartMontageAt*/0.f);

		if (!Dyn) return;

		if (!Anim->Montage_IsPlaying(Dyn))
		{
			const float Ret = Anim->Montage_Play(Dyn, 1.f); // Dyn 생성 시 PlayRate 반영됨
			if (Ret > 0.f && !Section.IsNone() && Dyn->IsValidSectionName(Section))
			{
				Anim->Montage_JumpToSection(Section, Dyn);
			}
		}
	}
}


void AOCRevenant::Server_RequestReload_Implementation()
{
	if (UAbilitySystemComponent* ASC = FindComponentByClass<UAbilitySystemComponent>())
	{
		const FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.RevReload"));
		const bool bOK = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
		UE_LOG(LogTemp, Warning, TEXT("[Server_RequestReload] TryActivate RevReload: %s"),
			bOK ? TEXT("Success") : TEXT("Fail"));
	}
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
