#include "GA/GA_RangedAttack.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "GameFramework/Character.h"
#include "GE/GE_RangedAttackCooldown.h"

#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"

UGA_RangedAttack::UGA_RangedAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CooldownGE = UGE_RangedAttackCooldown::StaticClass();
}

bool UGA_RangedAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo)
		return false;

	// 애니 준비 확인
	if (!ResolveMontage(ActorInfo))
		return false;

	// 쿨타임으로만 차단 (State 태그를 쓰지 않음)
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag("Cooldown.RangedAttack")))
	{
		return false;
	}

	if (const AOCRevenant* Rev = ActorInfo ? Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()) : nullptr)
	{
		if (Rev->IsAmmoEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[RangedAttack] No ammo -> try auto reload"));
			if (ActorInfo->IsNetAuthority())
			{
		           // 서버일 때: 서버가 바로 장전 GA 실행
				FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Reload"));
				ActorInfo->AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
			}
			else
			{
		           // 클라일 때: 서버에 "장전해 주세요" RPC
				if (AOCRevenant* M = const_cast<AOCRevenant*>(Rev))
				{
					M->Server_RequestReload();
				}
			}
			return false; // 발사 불가
		}
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_RangedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo->IsNetAuthority())
	{
		if (AOCRevenant* Rev = Cast<AOCRevenant>(ActorInfo->AvatarActor.Get()))
		{
			if (!Rev->ConsumeAmmo(1))
			{
				// 0발 등으로 실패: 자동 리로드
				UE_LOG(LogTemp, Warning, TEXT("[RangedAttack] ConsumeAmmo failed -> auto reload"));
				FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Reload"));
				ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReloadTag));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[RangedAttack] Fire OK. Ammo: %d/%d"), Rev->GetAmmo(), Rev->MaxAmmo);
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
		}
		// 태그는 GameplayCue 전용 태그여야 함
		ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Weapon.MuzzleFlash")),
			Params);
	}

	UAnimMontage* MontageToPlay = ResolveMontage(ActorInfo);
	ACharacter* Char = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	float MontageLen = 0.f;

	if (MontageToPlay && Char)
	{
		        // 재생 길이는 서버에서 계산(쿨타임 지속시간 덮어쓰기에 필요)
		MontageLen = MontageToPlay->GetPlayLength() / FMath::Max(PlayRate, 0.01f);
		
	        // 1) 소유자(내 화면) 즉시 체감: 로컬에서 선재생(있으면 유지)
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
					const UAnimSequenceBase* ReloadSrc = nullptr;
					const FName SlotName = DynamicMontageSlotName.IsNone() ? FName(TEXT("UpperSlot")) : DynamicMontageSlotName;
					Rev->Multicast_PlayReloadByAsset(const_cast<UAnimSequenceBase*>(ReloadSrc), SlotName, PlayRate, SectionName);
				}
			}

			if (ActorInfo->IsNetAuthority() && CooldownGE)
			{
				const float DesiredCD =
					FMath::Max(0.01f, CooldownSec.GetValueAtLevel(GetAbilityLevel(Handle, ActorInfo)));

				FGameplayEffectSpecHandle Cd = MakeOutgoingCooldownSpec(Handle, ActorInfo);
				if (Cd.IsValid() && Cd.Data.IsValid())
				{
					Cd.Data->SetDuration(FMath::Max(0.01f, MontageLen), /*bLockDuration=*/false);

					Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.RangedAttack")));
					Cd.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.ActionLock")));
					UE_LOG(LogTemp, Log, TEXT("[AttackCD] Desired=%.2f, SpecDuration=%.2f"), DesiredCD, Cd.Data->GetDuration());

					ASC->ApplyGameplayEffectSpecToSelf(*Cd.Data.Get());
				}
			}
			
		}		
	}
	// 파이어볼처럼 즉시 종료 (상태태그 잔류 X, 다음 발은 쿨타임이 막음)
	EndAbility(Handle, ActorInfo, ActivationInfo, /*rep*/true, /*cancel*/false);
}

UAnimMontage* UGA_RangedAttack::ResolveMontage(const FGameplayAbilityActorInfo* ActorInfo) const
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
						? FName(TEXT("UpperSlot")) : DynamicMontageSlotName;

					return UAnimMontage::CreateSlotAnimationAsDynamicMontage(
						S->PrimaryAttack, SlotName, 0.2f, 0.2f, PlayRate, /*Loop*/1, 0.f, 0.f);
				}
			}
		}
	}

	// 폴백
	return FallbackMontage;
}

FGameplayEffectSpecHandle UGA_RangedAttack::MakeOutgoingCooldownSpec(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!CooldownGE) return FGameplayEffectSpecHandle();
	return MakeOutgoingGameplayEffectSpec(CooldownGE, GetAbilityLevel(Handle, ActorInfo));
}
