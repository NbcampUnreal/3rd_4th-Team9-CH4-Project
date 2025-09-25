#include "Abilities/OCMarkComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "GameEffects/MarkVisualActor.h"
#include "Particles/ParticleSystemComponent.h"

UOCMarkComponent::UOCMarkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOCMarkComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupGlobalASCEvents();
    
	if (UWorld* World = GetWorld())
	{
		ActorSpawnedHandle = World->AddOnActorSpawnedHandler(
			FOnActorSpawned::FDelegate::CreateUFunction(this, FName("OnActorSpawned"))
		);
	}
}

void UOCMarkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto& MarkPair : ActiveMarks)
	{
		if (MarkPair.Value.IsValid())
		{
			MarkPair.Value->Destroy();
		}
	}
	ActiveMarks.Empty();
    
	if (UWorld* World = GetWorld())
	{
		World->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
	}
    
	Super::EndPlay(EndPlayReason);
}

void UOCMarkComponent::SetupGlobalASCEvents()
{
	if (!GetWorld()) return;
	int32 ActorCount = 0;
	int32 ASCCount = 0;
    
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorCount++;
		AActor* Actor = *ActorItr;
        
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
		if (ASC)
		{
			ASCCount++;
			UE_LOG(LogTemp, Warning, TEXT(" Binding to existing actor: %s"), *Actor->GetName());
            
			FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(TEXT("State.Marked"));
            
			ASC->RegisterGameplayTagEvent(MarkTag, EGameplayTagEventType::NewOrRemoved)
				.AddLambda([this, Actor](const FGameplayTag CallbackTag, int32 NewCount)
				{
					OnMarkTagChanged(CallbackTag, NewCount, Actor);
				});
		}
	}
    
	UE_LOG(LogTemp, Warning, TEXT(" Processed %d actors, found %d with ASC"), ActorCount, ASCCount);
}

void UOCMarkComponent::OnActorSpawned(AActor* SpawnedActor)
{
	if (!SpawnedActor) return;
    
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SpawnedActor);
	if (ASC)
	{
		FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(TEXT("State.Marked"));
        
		ASC->RegisterGameplayTagEvent(MarkTag, EGameplayTagEventType::NewOrRemoved)
			.AddLambda([this, SpawnedActor](const FGameplayTag CallbackTag, int32 NewCount)
			{
				OnMarkTagChanged(CallbackTag, NewCount, SpawnedActor);
			});
            
		UE_LOG(LogTemp, Log, TEXT("ASC event bound for: %s"), *SpawnedActor->GetName());
	}
}

void UOCMarkComponent::OnMarkTagChanged(const FGameplayTag CallbackTag, int32 NewCount, AActor* Actor)
{
	if (!Actor || !IsValid(Actor)) return;

	if (NewCount > 0)
	{
		if (MarkVisualActorClass && !ActiveMarks.Contains(Actor))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AMarkVisualActor* MarkActor = GetWorld()->SpawnActor<AMarkVisualActor>(
				MarkVisualActorClass,
				Actor->GetActorLocation(),
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (MarkActor)
			{
				MarkActor->SetTargetActor(Actor);
				ActiveMarks.Add(Actor, MarkActor);
                
				UE_LOG(LogTemp, Warning, TEXT("Auto mark created for: %s"), *Actor->GetName());
			}
		}
	}
	else
	{
		if (TWeakObjectPtr<AMarkVisualActor>* FoundMark = ActiveMarks.Find(Actor))
		{
			if (FoundMark->IsValid())
			{
				(*FoundMark)->RemoveMark();
			}
			ActiveMarks.Remove(Actor);

			UE_LOG(LogTemp, Warning, TEXT("Auto mark removed for: %s"), *Actor->GetName());
		}
	}
}
