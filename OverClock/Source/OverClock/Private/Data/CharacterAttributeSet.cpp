#include "Data/CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

#include "AbilitySystemComponent.h" 
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
	InitMaxHealth(100.f);
	InitHealth(GetMaxHealth());
}

void UCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Health, OldValue);
}

void UCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxHealth, OldValue);
}

void UCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    UE_LOG(LogTemp, Warning, TEXT("[Attr] PostGameplayEffectExecute ENTER: %s"), *Data.EvaluatedData.Attribute.GetName());

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float DeltaValue = Data.EvaluatedData.Magnitude;

        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

        UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
        AActor* TargetActor = GetOwningActor();                                 

        UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
        AActor* SourceActor = Data.EffectSpec.GetContext().GetOriginalInstigator(); 

        if (DeltaValue < 0.0f)
        {
            if (TargetASC && TargetActor)
            {
                UE_LOG(LogTemp, Warning, TEXT("DAMAGE CONFIRMED! Target: %s, Applied Damage: %f, Current Health: %f"), *TargetActor->GetName(), -DeltaValue, GetHealth());
            }
        }
    }

    if (GetHealth() <= 0.f)
    {
        if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
        {
            const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
            const FGameplayTag DiedEvt = FGameplayTag::RequestGameplayTag(TEXT("Event.Character.Died"));

            if (!ASC->HasMatchingGameplayTag(DeadTag))
            {
                ASC->AddLooseGameplayTag(DeadTag);

                FGameplayEventData Payload;
                UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), DiedEvt, Payload);

                UE_LOG(LogTemp, Log, TEXT("[Death] %s died (HP<=0). Event.Character.Died sent."), *GetOwningActor()->GetName());
            }
        }
    }
}
