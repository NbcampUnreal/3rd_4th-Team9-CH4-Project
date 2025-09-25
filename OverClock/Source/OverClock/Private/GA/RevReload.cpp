// RevReload.cpp
#include "GA/RevReload.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "GameFramework/Character.h"

#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"
#include "GE/GE_RevReload.h"
#include "TimerManager.h"

URevReload::URevReload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.RevReload")));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.RevReload")));

	CooldownGE = UGE_RevReload::StaticClass();
}

bool URevReload::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	// �ǽ�Ű�ۿ� �����ϰ�: ����� ��Ÿ��(�Ǵ� ������)�� �־�߸� Ȱ��
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
		return false;

	// �ִ� �غ� Ȯ��
	if (!ResolveReloadMontage(ActorInfo))
		return false;

	// ��Ÿ�����θ� ���� (State �±׸� ���� ����)
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag("Cooldown.RevReload")))
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void URevReload::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

	// ���/��Ÿ�� Ŀ�� (��Ÿ�� GE�� ������� true)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay = ResolveReloadMontage(ActorInfo);
	ACharacter* Char = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;

	float MontageLen = 0.f;

	// ����(������)������ ��� (�װ� ���� ������ ����)
	if ( ActorInfo->IsLocallyControlled())
	{
		if (MontageToPlay && Char)
		{
			if (UAnimInstance* Anim = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr)
			{
				MontageLen = MontageToPlay->GetPlayLength() / FMath::Max(PlayRate, 0.01f);

				const float Ret = Anim->Montage_Play(MontageToPlay, PlayRate);
				if (Ret > 0.f && !SectionName.IsNone() && MontageToPlay->IsValidSectionName(SectionName))
				{
					Anim->Montage_JumpToSection(SectionName, MontageToPlay);
				}
			}
		}
	}

	// ��Ÿ�� GE: �ǽ�Ű�ۿ� �����ϰ� "�ִ� ���� / PlayRate" �� Duration���� �����
	if (ActorInfo->IsNetAuthority() && CooldownGE && MontageLen > 0.f)
	{
		FGameplayEffectSpecHandle Cd = MakeOutgoingCooldownSpec(Handle, ActorInfo);
		if (Cd.IsValid())
		{
			// Duration ��Ÿ�� �������̵� + �±� �ο�
			Cd.Data->SetDuration(MontageLen, /*bLockDuration*/false);
			Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag("Cooldown.RevReload"));
			ASC->ApplyGameplayEffectSpecToSelf(*Cd.Data.Get());
		}
	}

	if (ActorInfo->IsNetAuthority())
	{
		if (AOCRevenant* Rev = Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()))
		{
			const float Delay = (MontageLen > 0.f) ? MontageLen : 0.f;
			if (UWorld* World = Rev->GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(Rev, [Rev, Delay]()
					{
						if (UWorld* W = Rev->GetWorld())
						{
							if (Delay > 0.f)
							{
								FTimerHandle Th;
								W->GetTimerManager().SetTimer(Th, FTimerDelegate::CreateWeakLambda(Rev, [Rev]()
									{
										Rev->RefillAmmo(); // �ִ� ������ ä��
									}), Delay, false);
							}
							else
							{
								Rev->RefillAmmo();
							}
						}
					}));
			}
		}
	}

	// �� ? �ǽ�Ű��ó�� ��� ���� (���� �±� ���� X)
	EndAbility(Handle, ActorInfo, ActivationInfo, /*replicateEnd*/true, /*wasCancelled*/false);
}

UAnimMontage* URevReload::ResolveReloadMontage(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const AOCRevenant* Rev = ActorInfo ? Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()) : nullptr)
	{
		// ĳ������ �����Ϳ���(�Ǵ� GA �������̵�)
		const UOCAnimDataAsset* Data = AnimDataAssetOverride ? AnimDataAssetOverride.Get() : Rev->GetAnimDataAsset();
		if (Data)
		{
			if (const FOCAnimStruct* S = Data->CharacterAnimations.Find(Rev->GetCharacterTypeTag()))
			{
				if (S && S->Reload)
				{
					// �̹� ��Ÿ�ָ� �״�� ���
					if (UAnimMontage* AsMontage = Cast<UAnimMontage>(S->Reload))
						return AsMontage;

					// �������� ���� ��Ÿ�ַ� ���α� (AnimBP�� Slot�� �̸� ��ġ �ʿ�)
					const FName SlotName = DynamicMontageSlotName.IsNone()
						? FName(TEXT("DefaultSlot")) : DynamicMontageSlotName;

					return UAnimMontage::CreateSlotAnimationAsDynamicMontage(
						S->Reload, SlotName, 0.2f, 0.2f, PlayRate, /*Loop*/1, 0.f, 0.f);
				}
			}
		}
	}

	// ����
	return ReloadFallbackMontage;
}

FGameplayEffectSpecHandle URevReload::MakeOutgoingCooldownSpec(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!CooldownGE) return FGameplayEffectSpecHandle();
	return MakeOutgoingGameplayEffectSpec(CooldownGE, GetAbilityLevel(Handle, ActorInfo));
}

