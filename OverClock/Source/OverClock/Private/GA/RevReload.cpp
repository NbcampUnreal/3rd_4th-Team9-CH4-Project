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
	// 피스키퍼와 동일하게: 재생할 몽타주(또는 시퀀스)가 있어야만 활성
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
		return false;

	// 애니 준비 확인
	if (!ResolveReloadMontage(ActorInfo))
		return false;

	// 쿨타임으로만 차단 (State 태그를 쓰지 않음)
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

	// 비용/쿨타임 커밋 (쿨타임 GE가 비었으면 true)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay = ResolveReloadMontage(ActorInfo);
	ACharacter* Char = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;

	float MontageLen = 0.f;

	// 로컬(소유자)에서만 재생 (네가 쓰는 구조와 동일)
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

	// 쿨타임 GE: 피스키퍼와 동일하게 "애니 길이 / PlayRate" 를 Duration으로 덮어쓰기
	if (ActorInfo->IsNetAuthority() && CooldownGE && MontageLen > 0.f)
	{
		FGameplayEffectSpecHandle Cd = MakeOutgoingCooldownSpec(Handle, ActorInfo);
		if (Cd.IsValid())
		{
			// Duration 런타임 오버라이드 + 태그 부여
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
										Rev->RefillAmmo(); // 애니 끝나고 채움
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

	// 끝 ? 피스키퍼처럼 즉시 종료 (상태 태그 상주 X)
	EndAbility(Handle, ActorInfo, ActivationInfo, /*replicateEnd*/true, /*wasCancelled*/false);
}

UAnimMontage* URevReload::ResolveReloadMontage(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const AOCRevenant* Rev = ActorInfo ? Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()) : nullptr)
	{
		// 캐릭터의 데이터에셋(또는 GA 오버라이드)
		const UOCAnimDataAsset* Data = AnimDataAssetOverride ? AnimDataAssetOverride.Get() : Rev->GetAnimDataAsset();
		if (Data)
		{
			if (const FOCAnimStruct* S = Data->CharacterAnimations.Find(Rev->GetCharacterTypeTag()))
			{
				if (S && S->Reload)
				{
					// 이미 몽타주면 그대로 사용
					if (UAnimMontage* AsMontage = Cast<UAnimMontage>(S->Reload))
						return AsMontage;

					// 시퀀스면 동적 몽타주로 감싸기 (AnimBP의 Slot과 이름 일치 필요)
					const FName SlotName = DynamicMontageSlotName.IsNone()
						? FName(TEXT("DefaultSlot")) : DynamicMontageSlotName;

					return UAnimMontage::CreateSlotAnimationAsDynamicMontage(
						S->Reload, SlotName, 0.2f, 0.2f, PlayRate, /*Loop*/1, 0.f, 0.f);
				}
			}
		}
	}

	// 폴백
	return ReloadFallbackMontage;
}

FGameplayEffectSpecHandle URevReload::MakeOutgoingCooldownSpec(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!CooldownGE) return FGameplayEffectSpecHandle();
	return MakeOutgoingGameplayEffectSpec(CooldownGE, GetAbilityLevel(Handle, ActorInfo));
}

