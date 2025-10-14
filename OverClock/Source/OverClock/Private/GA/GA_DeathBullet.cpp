#include "GA/GA_DeathBullet.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Component/WeaponAmmoComponent.h"

// ĳ���ͺ� ���� ��� ������(����)
#include "Player/OCCharacterBase.h" 
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"
#include "Player/OCPlayerController.h"

UGA_DeathBullet::UGA_DeathBullet()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    const FGameplayTag Lock = FGameplayTag::RequestGameplayTag(TEXT("State.ActionLock"));
    ActivationOwnedTags.AddTag(Lock);
    ActivationBlockedTags.AddTag(Lock);
}

void UGA_DeathBullet::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

    const AOCCharacterBase* Char = Cast<AOCCharacterBase>(ActorInfo->AvatarActor.Get());
    const FGameplayTag CharTag = Char->GetCurrentTag();

	const FOCAnimStruct* AS = Char->GetAnimDataAsset()->CharacterAnimations.Find(CharTag);

	UAnimMontage* Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AS->Skill1, DynamicMontageSlotName, 0.2f, 0.2f, PlayRate, 1, 0.f, 0.f);

	auto* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, PlayRate, NAME_None, true);

	Task->OnBlendOut.AddDynamic(this, &UGA_DeathBullet::OnMontageCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_DeathBullet::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_DeathBullet::OnMontageInterrupted);
	Task->OnCancelled.AddDynamic(this, &UGA_DeathBullet::OnMontageInterrupted);
	Task->ReadyForActivation();
}

void UGA_DeathBullet::OnMontageCompleted()
{
    if (CurrentActorInfo->IsNetAuthority())
    {
    	//병합 충돌로 인한 수정
    	//AOCPlayerController* PC = Cast<AOCPlayerController>(CurrentActorInfo->PlayerController.Get());
    	//UAbilitySystemComponent* ASC = PC->GetASC();
    	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        if (!ASC)
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); return;
        }

        const int32 Level = GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo);

        FGameplayEffectSpecHandle Handle = MakeOutgoingGameplayEffectSpec(DeathBulletWindowGE, Level);
        if (Handle.IsValid() && Handle.Data.IsValid())
        {
            FGameplayEffectSpec* Spec = Handle.Data.Get();

            if (Data_DurationTag.IsValid())
                Spec->SetSetByCallerMagnitude(Data_DurationTag, WindowDuration);

            Spec->Duration = WindowDuration;

            Spec->StackCount = FMath::Max(1, EmpoweredShots);

            if (WindowTag.IsValid())
                Spec->DynamicGrantedTags.AddTag(WindowTag);

            WindowHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);

            if (WindowHandle.IsValid())
            {
                ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UGA_DeathBullet::OnAnyEffectRemoved);
            }
        }
    }
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DeathBullet::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_DeathBullet::OnAnyEffectRemoved(const FActiveGameplayEffect& Removed)
{
    if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;

    if (Removed.Handle == WindowHandle)
    {
        ApplyCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
        WindowHandle = FActiveGameplayEffectHandle();
    }
}

void UGA_DeathBullet::EndAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        const FGameplayTag Lock = FGameplayTag::RequestGameplayTag(TEXT("State.ActionLock"));
        if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Lock))
        {
            ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(Lock);
        }
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
