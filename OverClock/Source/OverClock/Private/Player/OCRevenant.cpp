// OCRevenant.cpp
#include "Player/OCRevenant.h"
#include "Player/OCPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
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
		if (HasAuthority()) GiveStartupAbilities();
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

void AOCRevenant::GiveStartupAbilities()
{
	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = FindComponentByClass<UAbilitySystemComponent>())
	{
		for (const TPair<FGameplayTag, TSubclassOf<UGameplayAbility>>& Pair : AbilityMapByTag)
		{
			if (!Pair.Value) continue;

			FGameplayAbilitySpec Spec(Pair.Value, /*Level*/1, /*InputID*/INDEX_NONE, /*SourceObject*/this);

			Spec.DynamicAbilityTags.AddTag(Pair.Key);

			ASC->GiveAbility(Spec);
		}
	}
}
