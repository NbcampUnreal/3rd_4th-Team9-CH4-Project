#include "SubSystem/OCMarkSystemManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameEffects/MarkVisualActor.h"

void UOCMarkSystemManager::CreateMarkVisual(AActor* TargetActor)
{
    if (!TargetActor || !MarkVisualActorClass) return;

    // 이미 표식이 있으면 제거
    RemoveMarkVisual(TargetActor);

    // 새 표식 시각 액터 생성
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = nullptr; // 독립적으로 동작
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AMarkVisualActor* MarkActor = GetWorld()->SpawnActor<AMarkVisualActor>(
        MarkVisualActorClass,
        TargetActor->GetActorLocation(),
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (MarkActor)
    {
        MarkActor->SetTargetActor(TargetActor);
        ActiveMarks.Add(TargetActor, MarkActor);
        
        UE_LOG(LogTemp, Warning, TEXT("Mark visual created for: %s"), *TargetActor->GetName());
    }
}

void UOCMarkSystemManager::RemoveMarkVisual(AActor* TargetActor)
{
    if (!TargetActor) return;

    if (TWeakObjectPtr<AMarkVisualActor>* FoundMark = ActiveMarks.Find(TargetActor))
    {
        if (FoundMark->IsValid())
        {
            (*FoundMark)->RemoveMark();
        }
        ActiveMarks.Remove(TargetActor);
        
        UE_LOG(LogTemp, Warning, TEXT("Mark visual removed for: %s"), *TargetActor->GetName());
    }
}

void UOCMarkSystemManager::BindGASEvents(AActor* Actor)
{
    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
    if (ASC)
    {
        FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(TEXT("Status.Marked"));
        ASC->RegisterGameplayTagEvent(MarkTag, EGameplayTagEventType::NewOrRemoved)
            .AddUFunction(this, FName("OnActorTagChanged"));
    }
}

void UOCMarkSystemManager::OnActorTagChanged(AActor* Actor, const FGameplayTag& Tag, int32 NewCount)
{
    if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Marked"))))
    {
        if (NewCount > 0)
        {
            CreateMarkVisual(Actor);
        }
        else
        {
            RemoveMarkVisual(Actor);
        }
    }
}