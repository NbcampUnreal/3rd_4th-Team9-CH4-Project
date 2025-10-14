// Reload.cpp
#include "GA/GA_Reload.h"

#include "AbilitySystemComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "GameFramework/Character.h"

#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"
#include "GE/GE_RangedAttackCooldown.h"
#include "TimerManager.h"

UReload::UReload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CooldownGE = UGE_RangedAttackCooldown::StaticClass();
}

bool UReload::CanActivateAbility(
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
		FGameplayTag::RequestGameplayTag("Cooldown.Reload")))
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UReload::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

	UAnimMontage* MontageToPlay = ResolveReloadMontage(ActorInfo);
	ACharacter * Char = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	float MontageLen = 0.f;

	if (MontageToPlay && Char)
	{
		 // 재생 길이는 서버에서 계산(쿨타임 지속시간 덮어쓰기에 필요)  :contentReference[oaicite:2]{index=2}
		MontageLen = MontageToPlay->GetPlayLength() / FMath::Max(PlayRate, 0.01f);

		 // 1) 소유자(내 화면) 즉시 체감: 로컬에서 선재생(있으면 유지)  :contentReference[oaicite:3]{index=3}
		if (ActorInfo->IsLocallyControlled())
		{
			if (UAnimInstance* Anim = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr)
			{
				if (!Anim->Montage_IsPlaying(MontageToPlay))
				{
					const float PlayRet = Anim->Montage_Play(MontageToPlay, PlayRate);
					if (PlayRet > 0.f && !SectionName.IsNone() && MontageToPlay->IsValidSectionName(SectionName))
					{
						Anim->Montage_JumpToSection(SectionName, MontageToPlay);
					}
				}
			}
		}
		// 2) 서버에서 전체에게 확정 브로드캐스트(소유자 포함, 중복재생 가드됨)  :contentReference[oaicite:4]{index=4}
		if (ActorInfo->IsNetAuthority())
		{
			if (AOCRevenant* Rev = Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()))
			{
				const bool bIsTransient = MontageToPlay->GetPathName().StartsWith(TEXT("/Engine/Transient"));
				if (!bIsTransient)
				{
					Rev->Multicast_PlayMontage(MontageToPlay, PlayRate, SectionName);
				}
				else
				{
			        // 동적 몽타주는 포인터 전송 불가 → 원본 애니로 재구성 멀티캐스트  :contentReference[oaicite:5]{index=5}
					const UAnimSequenceBase * ReloadSrc = nullptr; // TODO: Resolve 시 원본 시퀀스 캐시해 전달
					const FName SlotName = DynamicMontageSlotName.IsNone() ? FName(TEXT("UpperSlot")) : DynamicMontageSlotName;
					Rev->Multicast_PlayReloadByAsset(const_cast<UAnimSequenceBase*>(ReloadSrc), SlotName, PlayRate, SectionName);
				}
			}
		}
	}

	// 게임 플레이 큐 실행
	if (ASC)
	{
		FGameplayCueParameters Params;
		if (const ACharacter* C = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			Params.SourceObject = C->GetMesh();
		}
		ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.Reload")), Params);
	}

	// 총알 장전
	if (ActorInfo->IsNetAuthority())
	{
		if (AOCRevenant* Rev = Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()))
		{
			const float Delay = (MontageLen > 0.f) ? MontageLen : 0.f;

			if (UWorld* World = Rev->GetWorld())
			{
				// 애니가 있으면 끝날 때에 맞춰, 없으면 즉시
				if (Delay > 0.f)
				{
					FTimerHandle Th;
					World->GetTimerManager().SetTimer(
						Th,
						FTimerDelegate::CreateWeakLambda(Rev, [Rev]()
						{
							Rev->RefillAmmo(); // 서버에서 채우면 CurrentAmmo가 복제됨
						}),
						Delay,
						false
					);
				}
				else
				{
					Rev->RefillAmmo();
				}
			}
		}
	}

	// 쿨타임 GE: 피스키퍼와 동일하게 "애니 길이 / PlayRate" 를 Duration으로 덮어쓰기
	// 쿹타임이 애니메이션 길이만큼 적용하려는데 안됨. 
	// 이게 되면 모든 스킬을 하나의 쿨타임GE로 사용가능.
	if (ActorInfo->IsNetAuthority() && CooldownGE)
	{
		const float DesiredCD = FMath::Max(0.01f, CooldownSec.GetValueAtLevel(GetAbilityLevel(Handle, ActorInfo)));


		FGameplayEffectSpecHandle Cd = MakeOutgoingCooldownSpec(Handle, ActorInfo);
		if (Cd.IsValid() && Cd.Data.IsValid())
		{
			Cd.Data->SetDuration(FMath::Max(0.01f, MontageLen), /*bLockDuration=*/false);

			// Reload 전용/공통 태그를 동적으로 부여
			Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Reload")));
			Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.ActionLock")));

			UE_LOG(LogTemp, Log, TEXT("[ReloadCD] Desired=%.2f, SpecDuration=%.2f"), DesiredCD, Cd.Data->GetDuration());

			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Cd.Data.Get());
		}
	}

	// 끝 ? 피스키퍼처럼 즉시 종료 (상태 태그 상주 X)
	EndAbility(Handle, ActorInfo, ActivationInfo, /*replicateEnd*/true, /*wasCancelled*/false);
}

UAnimMontage* UReload::ResolveReloadMontage(const FGameplayAbilityActorInfo* ActorInfo) const
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
						? FName(TEXT("UpperSlot")) : DynamicMontageSlotName;

					return UAnimMontage::CreateSlotAnimationAsDynamicMontage(
						S->Reload, SlotName, 0.2f, 0.2f, PlayRate, /*Loop*/1, 0.f, 0.f);
				}
			}
		}
	}

	// 폴백
	return ReloadFallbackMontage;
}

FGameplayEffectSpecHandle UReload::MakeOutgoingCooldownSpec(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!CooldownGE) return FGameplayEffectSpecHandle();
	return MakeOutgoingGameplayEffectSpec(CooldownGE, GetAbilityLevel(Handle, ActorInfo));
}

