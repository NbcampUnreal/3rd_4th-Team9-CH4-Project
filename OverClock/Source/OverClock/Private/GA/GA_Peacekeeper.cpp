#include "GA/GA_Peacekeeper.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "GameFramework/Character.h"
#include "GE/PeacekeeperCooldown.h"

#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"

UGA_Peacekeeper::UGA_Peacekeeper()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.PeaceKeeper")));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.PeaceKeeper")));

	CooldownGE = UPeacekeeperCooldown::StaticClass();
}

bool UGA_Peacekeeper::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
		return false;

	// �ִ� �غ� Ȯ��
	if (!ResolveMontage(ActorInfo))
		return false;

	// ��Ÿ�����θ� ���� (State �±׸� ���� ����)
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag("Cooldown.PeaceKeeper")))
	{
		return false;
	}

	if (const AOCRevenant* Rev = ActorInfo ? Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()) : nullptr)
	{
		if (Rev->IsAmmoEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Peacekeeper] No ammo -> try auto reload"));
			if (ActorInfo->IsNetAuthority())
			{
				FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.RevReload"));
				ActorInfo->AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
			}
			return false; // �߻� �Ұ�
		}
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Peacekeeper::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

	if (ActorInfo->IsNetAuthority())
	{
		if (AOCRevenant* Rev = Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()))
		{
			if (!Rev->ConsumeAmmo(1))
			{
				// 0�� ������ ����: �ڵ� ���ε�
				UE_LOG(LogTemp, Warning, TEXT("[Peacekeeper] ConsumeAmmo failed -> auto reload"));
				FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.RevReload"));
				ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[Peacekeeper] Fire OK. Ammo: %d/%d"), Rev->GetAmmo(), Rev->MaxAmmo);
			}
		}
	}

	if (ASC)
	{
		FGameplayCueParameters Params;

		// ���� ������ ���� Mesh�� SourceObject�� �ѱ�� GCN���� ��Ȯ�� �� ������Ʈ�� ����
		if (const ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			// ���Ⱑ ĳ���� ���̷��濡 �پ� �ִٸ�, ĳ���� Mesh�� ���
			Params.SourceObject = Char->GetMesh();
		}

		// (����) ��ġ/���⵵ �ѱ���� ���� Ʈ�������� ��� ä���� �� ����
		// UE5.5: SourceObject�� const UObject* �̹Ƿ� const�� ĳ���� �� �б� �������� ���
		if (const UObject* SO = Params.SourceObject.Get())
		{
			if (const USkeletalMeshComponent* Sk = Cast<const USkeletalMeshComponent>(SO))
			{
				if (Sk->DoesSocketExist(TEXT("Muzzle")))
				{
					const FTransform MuzzleXf = Sk->GetSocketTransform(TEXT("Muzzle"));
					Params.Location = MuzzleXf.GetLocation();
					Params.Normal = MuzzleXf.GetRotation().GetForwardVector();
				}
			}
		}

		// �±״� GameplayCue ���� �±׿��� ��
		ASC->ExecuteGameplayCue(
			FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.RevMuzzleFlash")),
			Params);
	}

	// ���/��Ÿ�� Ŀ�� (��Ÿ�� GE�� ������� true)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay = ResolveMontage(ActorInfo);
	ACharacter* Char = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;

	float MontageLen = 0.f;

	if (ActorInfo->IsLocallyControlled())
	{
		if (MontageToPlay && Char)
		{
			if (UAnimInstance* Anim = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr)
			{
				MontageLen = MontageToPlay->GetPlayLength() / FMath::Max(PlayRate, 0.01f);

				const float PlayRet = Anim->Montage_Play(MontageToPlay, PlayRate);
				if (PlayRet > 0.f && !SectionName.IsNone() && MontageToPlay->IsValidSectionName(SectionName))
				{
					Anim->Montage_JumpToSection(SectionName, MontageToPlay);
				}
			}
		}
	}
	
		// ��Ÿ�� GE ����: ��Ÿ�� ��� �ð���ŭ (������ ��ŵ)
	if (ActorInfo->IsNetAuthority() && CooldownGE && MontageLen > 0.f)
	{
		FGameplayEffectSpecHandle Cd = MakeOutgoingCooldownSpec(Handle, ActorInfo);
		if (Cd.IsValid())
		{
			// Duration ��Ÿ�� �������̵� + �±� �ο�
			Cd.Data->SetDuration(MontageLen, /*bLockDuration*/false);
			Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag("Cooldown.PeaceKeeper"));
			ASC->ApplyGameplayEffectSpecToSelf(*Cd.Data.Get());
		}
	}
	

	// ���̾ó�� ��� ���� (�����±� �ܷ� X, ���� ���� ��Ÿ���� ����)
	EndAbility(Handle, ActorInfo, ActivationInfo, /*rep*/true, /*cancel*/false);
}

UAnimMontage* UGA_Peacekeeper::ResolveMontage(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const AOCRevenant* Rev = ActorInfo ? Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()) : nullptr)
	{
		// ĳ������ �����Ϳ���(�Ǵ� GA �������̵�)
		const UOCAnimDataAsset* Data = AnimDataAssetOverride ? AnimDataAssetOverride.Get() : Rev->GetAnimDataAsset();
		if (Data)
		{
			if (const FOCAnimStruct* S = Data->CharacterAnimations.Find(Rev->GetCharacterTypeTag()))
			{
				if (S && S->PrimaryAttack)
				{
					// �̹� ��Ÿ�ָ� �״�� ���
					if (UAnimMontage* AsMontage = Cast<UAnimMontage>(S->PrimaryAttack))
						return AsMontage;

					// �������� ���� ��Ÿ�ַ� ���α� (AnimBP�� Slot�� �̸� ��ġ �ʿ�)
					const FName SlotName = DynamicMontageSlotName.IsNone()
						? FName(TEXT("DefaultSlot")) : DynamicMontageSlotName;

					return UAnimMontage::CreateSlotAnimationAsDynamicMontage(
						S->PrimaryAttack, SlotName, 0.2f, 0.2f, PlayRate, /*Loop*/1, 0.f, 0.f);
				}
			}
		}
	}

	// ����
	return FallbackMontage;
}

FGameplayEffectSpecHandle UGA_Peacekeeper::MakeOutgoingCooldownSpec(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!CooldownGE) return FGameplayEffectSpecHandle();
	return MakeOutgoingGameplayEffectSpec(CooldownGE, GetAbilityLevel(Handle, ActorInfo));
}
