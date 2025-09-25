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

	// 애니 준비 확인
	if (!ResolveMontage(ActorInfo))
		return false;

	// 쿨타임으로만 차단 (State 태그를 쓰지 않음)
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
			return false; // 발사 불가
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
				// 0발 등으로 실패: 자동 리로드
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

		// 머즐 소켓이 붙은 Mesh를 SourceObject로 넘기면 GCN에서 정확히 그 컴포넌트에 붙임
		if (const ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			// 무기가 캐릭터 스켈레톤에 붙어 있다면, 캐릭터 Mesh로 충분
			Params.SourceObject = Char->GetMesh();
		}

		// (선택) 위치/방향도 넘기려면 소켓 트랜스폼을 얻어 채워줄 수 있음
		// UE5.5: SourceObject는 const UObject* 이므로 const로 캐스팅 후 읽기 전용으로 사용
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

		// 태그는 GameplayCue 전용 태그여야 함
		ASC->ExecuteGameplayCue(
			FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.RevMuzzleFlash")),
			Params);
	}

	// 비용/쿨타임 커밋 (쿨타임 GE가 비었으면 true)
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
	
		// 쿨타임 GE 적용: 몽타주 재생 시간만큼 (없으면 스킵)
	if (ActorInfo->IsNetAuthority() && CooldownGE && MontageLen > 0.f)
	{
		FGameplayEffectSpecHandle Cd = MakeOutgoingCooldownSpec(Handle, ActorInfo);
		if (Cd.IsValid())
		{
			// Duration 런타임 오버라이드 + 태그 부여
			Cd.Data->SetDuration(MontageLen, /*bLockDuration*/false);
			Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag("Cooldown.PeaceKeeper"));
			ASC->ApplyGameplayEffectSpecToSelf(*Cd.Data.Get());
		}
	}
	

	// 파이어볼처럼 즉시 종료 (상태태그 잔류 X, 다음 발은 쿨타임이 막음)
	EndAbility(Handle, ActorInfo, ActivationInfo, /*rep*/true, /*cancel*/false);
}

UAnimMontage* UGA_Peacekeeper::ResolveMontage(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const AOCRevenant* Rev = ActorInfo ? Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()) : nullptr)
	{
		// 캐릭터의 데이터에셋(또는 GA 오버라이드)
		const UOCAnimDataAsset* Data = AnimDataAssetOverride ? AnimDataAssetOverride.Get() : Rev->GetAnimDataAsset();
		if (Data)
		{
			if (const FOCAnimStruct* S = Data->CharacterAnimations.Find(Rev->GetCharacterTypeTag()))
			{
				if (S && S->PrimaryAttack)
				{
					// 이미 몽타주면 그대로 사용
					if (UAnimMontage* AsMontage = Cast<UAnimMontage>(S->PrimaryAttack))
						return AsMontage;

					// 시퀀스면 동적 몽타주로 감싸기 (AnimBP의 Slot과 이름 일치 필요)
					const FName SlotName = DynamicMontageSlotName.IsNone()
						? FName(TEXT("DefaultSlot")) : DynamicMontageSlotName;

					return UAnimMontage::CreateSlotAnimationAsDynamicMontage(
						S->PrimaryAttack, SlotName, 0.2f, 0.2f, PlayRate, /*Loop*/1, 0.f, 0.f);
				}
			}
		}
	}

	// 폴백
	return FallbackMontage;
}

FGameplayEffectSpecHandle UGA_Peacekeeper::MakeOutgoingCooldownSpec(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!CooldownGE) return FGameplayEffectSpecHandle();
	return MakeOutgoingGameplayEffectSpec(CooldownGE, GetAbilityLevel(Handle, ActorInfo));
}
