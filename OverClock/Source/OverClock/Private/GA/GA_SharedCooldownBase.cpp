#include "GA/GA_SharedCooldownBase.h"

// ¸í½ÃÀû include
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

UGA_SharedCooldownBase::UGA_SharedCooldownBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetByCaller_DurationKey = FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown.Duration"));

    CooldownSeconds = FScalableFloat(5.0f);
}

bool UGA_SharedCooldownBase::BuildCooldownSpec(
    FGameplayEffectSpecHandle& OutSpecHandle,
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo
) const
{
    if (!SharedCooldownGE || !ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        return false;
    }

    const int32 Level = GetAbilityLevel(Handle, ActorInfo);

    OutSpecHandle = MakeOutgoingGameplayEffectSpec(SharedCooldownGE, Level);

    if (!OutSpecHandle.IsValid() || !OutSpecHandle.Data.IsValid())
    {
        return false;
    }

    FGameplayEffectSpec* Spec = OutSpecHandle.Data.Get();

    const float DurationSec = CooldownSeconds.GetValueAtLevel(Level);

    if (SetByCaller_DurationKey.IsValid())
    {
        Spec->SetSetByCallerMagnitude(SetByCaller_DurationKey, DurationSec);
    }

    if (CooldownTag.IsValid())
    {
        Spec->DynamicGrantedTags.AddTag(CooldownTag);
    }

    return true;
}

void UGA_SharedCooldownBase::ApplyCooldown(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (!ActorInfo->AbilitySystemComponent.IsValid())
    {
        return;
    }

    FGameplayEffectSpecHandle SpecHandle;
    if (!BuildCooldownSpec(SpecHandle, Handle, ActorInfo))
    {
        return;
    }

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

const FGameplayTagContainer* UGA_SharedCooldownBase::GetCooldownTags() const
{
    CooldownTagsCache.Reset();
    if (CooldownTag.IsValid())
    {
        CooldownTagsCache.AddTag(CooldownTag);
    }
    return &CooldownTagsCache;
}

UGameplayEffect* UGA_SharedCooldownBase::GetCooldownGameplayEffect() const
{
    return SharedCooldownGE->GetDefaultObject<UGameplayEffect>();
}

void UGA_SharedCooldownBase::SetCooldownSeconds(float InSeconds)
{
    CooldownSeconds = FScalableFloat(FMath::Max(0.f, InSeconds));
}

void UGA_SharedCooldownBase::SetCooldownTag(FGameplayTag InTag)
{
    CooldownTag = InTag;
}

void UGA_SharedCooldownBase::SetSharedCooldownGE(TSubclassOf<UGameplayEffect> InGE)
{
    SharedCooldownGE = InGE;
}

void UGA_SharedCooldownBase::SetSetByCallerDurationKey(FGameplayTag InKey)
{
    SetByCaller_DurationKey = InKey;
}
