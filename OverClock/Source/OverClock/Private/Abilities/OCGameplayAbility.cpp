#include "Abilities/OCGameplayAbility.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "TimerManager.h"
#include "Data/OCGameplayTags.h"
#include "GameFramework/Character.h"

UOCGameplayAbility::UOCGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UOCAbilitySystemComponent* UOCGameplayAbility::GetOCASC() const
{
	return Cast<UOCAbilitySystemComponent>(CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent : nullptr);
}

ACharacter* UOCGameplayAbility::GetOCCharacter() const
{
	return Cast<ACharacter>(GetAvatarActorFromActorInfo());
}

bool UOCGameplayAbility::IsServerAuthority() const
{
	return GetCurrentActorInfo() && GetCurrentActorInfo()->IsNetAuthority();
}

UAbilityTask_PlayMontageAndWait* UOCGameplayAbility::PlayMontageTask(UAnimMontage* Montage, float PlayRate,
	FName StartSection, bool bStopWhenAbilityEnds, float RootMotionScale, float StartTimeSeconds,
	bool bAllowInterruptAfterBlendOut) const
{
	if (!Montage) return nullptr;
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			const_cast<UOCGameplayAbility*>(this),
			NAME_None,
			Montage,
			PlayRate,
			StartSection,
			bStopWhenAbilityEnds,
			RootMotionScale,
			StartTimeSeconds,
			bAllowInterruptAfterBlendOut
		);

	if (Task)
	{
		Task->ReadyForActivation();
	}
	return Task;
}

void UOCGameplayAbility::ApplyDamageToActor(AActor* TargetActor, float Damage, const FHitResult* OptionalHit) const
{
	if (!IsServerAuthority() || !TargetActor || Damage <= 0.f || !DefaultDamageGE) return;
	
	if (!CanDamageTarget(TargetActor)) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if (TargetASC)
	{
		ApplyDamageToASC(TargetASC, Damage, OptionalHit);
	}
}

void UOCGameplayAbility::ApplyDamageToASC(UAbilitySystemComponent* TargetASC, float Damage,
	const FHitResult* OptionalHit) const
{
	if (!IsServerAuthority() || !TargetASC || Damage <= 0.f || !DefaultDamageGE) return;
	
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;
	
	FGameplayEffectSpecHandle Spec = BuildDamageSpec(SourceASC, Damage, OptionalHit);
	if (Spec.IsValid())
	{
		SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	}
}


void UOCGameplayAbility::ApplySelfGE_Server(TSubclassOf<UGameplayEffect> EffectClass) const
{
	if (!IsServerAuthority() || !EffectClass) return;

	UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC == nullptr) return;

	ASC->ApplyGameplayEffectToSelf(EffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, ASC->MakeEffectContext());
	
}

void UOCGameplayAbility::AddLooseTag(const FGameplayTag& Tag) const
{
	UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC != nullptr)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UOCGameplayAbility::RemoveLooseTag(const FGameplayTag& Tag) const
{
	UOCAbilitySystemComponent* ASC = GetOCASC();
	if (ASC != nullptr)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

void UOCGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActivationPolicy == EOCAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UOCGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActivationPolicy == EOCAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

FGameplayEffectSpecHandle UOCGameplayAbility::BuildDamageSpec(UAbilitySystemComponent* SourceASC, float Damage,
	const FHitResult* OptionalHit) const
{
	FGameplayEffectSpecHandle Spec;

	if (!SourceASC || Damage <= 0.f || !DefaultDamageGE)
	{
		return Spec;
	}
	
	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		Ctx.AddSourceObject(Avatar);
		Ctx.AddInstigator(Avatar, Avatar);
	}
	if (OptionalHit)
	{
		Ctx.AddHitResult(*OptionalHit);
	}

	Spec = SourceASC->MakeOutgoingSpec(DefaultDamageGE, GetAbilityLevel(), Ctx);
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(OCGameplayTags::Data_Damage, Damage);
	}
	return Spec;
}
