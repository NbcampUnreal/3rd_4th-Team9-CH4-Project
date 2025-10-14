// AOCCharacterBase.cpp
#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

AOCCharacterBase::AOCCharacterBase()
	: WalkSpeed(600.f)
	, RunSpeed(900.f)
	, JumpVelocity(600.f)
	, AimRotation(FRotator::ZeroRotator)
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), TEXT("Eyes_Position"));
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArm);
	CameraComp->bUsePawnControlRotation = true;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
}

void AOCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AOCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		ServerSetAimRotation_Implementation(GetControlRotation());
	}
}

UAbilitySystemComponent* AOCCharacterBase::GetAbilitySystemComponent() const
{
	if (const AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AOCCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
		if (HasAuthority())
		{
			GiveStartupAbilities();
		}
	}
}

void AOCCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
	}
}

void AOCCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOCCharacterBase, AimRotation);
}

void AOCCharacterBase::ServerSetAimRotation_Implementation(FRotator InAimRotation)
{
	AimRotation = InAimRotation;
}

TSubclassOf<UGameplayAbility> AOCCharacterBase::GetAbilityClassByTag(FGameplayTag AbilityTag) const
{
	if (const TSubclassOf<UGameplayAbility>* Found = AbilityMapByTag.Find(AbilityTag))
	{
		return *Found;
	}
	return nullptr;
}

void AOCCharacterBase::GiveStartupAbilities()
{
	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (const TPair<FGameplayTag, TSubclassOf<UGameplayAbility>>& Pair : AbilityMapByTag)
		{
			if (!Pair.Value) continue;

			FGameplayAbilitySpec Spec(Pair.Value, 1, INDEX_NONE, this);
			Spec.DynamicAbilityTags.AddTag(Pair.Key);

			ASC->GiveAbility(Spec);
		}
	}
}
// ─────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────
// [공통화] 몽타주 재생 멀티캐스트
void AOCCharacterBase::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, float InPlayRate, FName InSection)
{
	if (!Montage) return;

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
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
}

void AOCCharacterBase::Multicast_PlaySequenceAsDynamicMontage_Implementation(
	UAnimSequenceBase* Source, FName SlotName, float InPlayRate, FName Section)
{
	if (!Source) return;

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
		{
			UAnimMontage* Dyn = UAnimMontage::CreateSlotAnimationAsDynamicMontage(
				Source, SlotName, /*BlendIn*/0.2f, /*BlendOut*/0.2f,
				/*PlayRate*/FMath::Max(InPlayRate, 0.01f),
				/*LoopCount*/1, /*BlendOutTriggerTime*/0.f, /*InTimeToStartMontageAt*/0.f);

			if (!Dyn) return;

			if (!Anim->Montage_IsPlaying(Dyn))
			{
				const float Ret = Anim->Montage_Play(Dyn, 1.f);
				if (Ret > 0.f && !Section.IsNone() && Dyn->IsValidSectionName(Section))
				{
					Anim->Montage_JumpToSection(Section, Dyn);
				}
			}
		}
	}
}
