#include "GA/GA_DeathBullet.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"

#include "Player/OCRevenant.h"
#include "Player/Anim/OCAnimDataAsset.h"
#include "Player/Anim/OCAnimStruct.h"
#include "GE/GE_RangedAttackCooldown.h"

UGA_DeathBullet::UGA_DeathBullet()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CooldownGE = UGE_RangedAttackCooldown::StaticClass();

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

bool UGA_DeathBullet::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayTagContainer* SourceTags, 
	const FGameplayTagContainer* TargetTags, 
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo) return false;

	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.DeathBullet"))))
	{
		return false;
	}

	if (!ReloadAnim || !FireBAnim || !FireCAnim) return false;

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_DeathBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); }

	ApplyCooldownWithTags(Handle, ActorInfo, CooldownSeconds);

	PlayAnim_ServerMulticast(ReloadAnim);

	bTriggered = false;
}

void UGA_DeathBullet::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
}

UAnimMontage* UGA_DeathBullet::MakeDynamicMontage(const UAnimSequenceBase* Seq) const
{
	return nullptr;
}

void UGA_DeathBullet::PlayAnim_ServerMulticast(const UAnimSequenceBase* Seq) const
{
}

void UGA_DeathBullet::PlaymuzzleCue(const FGameplayAbilityActorInfo* ActorInfo) const
{
}

void UGA_DeathBullet::OnPressedCallback(float TimeWaited)
{
}

void UGA_DeathBullet::ApplyCooldownWithTags(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float DurationSec) const
{
}
