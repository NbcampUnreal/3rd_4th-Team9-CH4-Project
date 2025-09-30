#include "Abilities/OCMarkComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraSystem.h"

UOCMarkComponent::UOCMarkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetCastShadow(false);
	SetVisibility(false); // 기본적으로 숨김
	SetRelativeLocation(MarkOffset);
	SetRelativeRotation(FRotator(180, 0, 0));

	// 파티클 효과들
	//MarkEffect = CreateDefaultSubobject<UNiagaraSystem>(TEXT("MarkEffect"));
}

void UOCMarkComponent::BeginPlay()
{
	Super::BeginPlay();
    
	// Component가 붙은 캐릭터의 ASC에 이벤트 등록
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerPawn))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				// Owner ASC에 MarkTag 이벤트 등록. 중복 X
				ASC->RegisterGameplayTagEvent(
					FGameplayTag::RequestGameplayTag("State.Marked"),
					EGameplayTagEventType::NewOrRemoved
				).AddUObject(this, &UOCMarkComponent::OnTagChanged);
			}
		}
	}
}

void UOCMarkComponent::OnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (CallbackTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Marked")))
	{
		// 틱 활성화
		SetComponentTickEnabled(true);
		SetVisibility(true);
	}
	else
	{
		// 틱 비활성화
		SetComponentTickEnabled(false);
		SetVisibility(false);
	}
}

void UOCMarkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
	// // 상하 움직임 애니메이션
	// BobTime += DeltaTime;
	// float BobOffset = FMath::Sin(BobTime * BobSpeed) * BobAmplitude;
 //    
	// if (MarkIndicator->IsVisible())
	// {
	// 	FVector NewLocation = MarkOffset;
	// 	NewLocation.Z += BobOffset;
	// 	MarkIndicator->SetRelativeLocation(NewLocation);
 //        
	// 	// 회전도 추가
	// 	FRotator NewRotation = MarkIndicator->GetRelativeRotation();
	// 	NewRotation.Yaw += 90.0f * DeltaTime; // 초당 90도 회전
	// 	MarkIndicator->SetRelativeRotation(NewRotation);
	// }
}


// void UOCMarkComponent::BeginPlay()
// {
// 	Super::BeginPlay();
//
// 	SetupGlobalASCEvents();
//     
// 	if (UWorld* World = GetWorld())
// 	{
// 		ActorSpawnedHandle = World->AddOnActorSpawnedHandler(
// 			FOnActorSpawned::FDelegate::CreateUFunction(this, FName("OnActorSpawned"))
// 		);
// 	}
// }
//
// void UOCMarkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
// {
// 	for (auto& MarkPair : ActiveMarks)
// 	{
// 		if (MarkPair.Value.IsValid())
// 		{
// 			MarkPair.Value->Destroy();
// 		}
// 	}
// 	ActiveMarks.Empty();
//     
// 	if (UWorld* World = GetWorld())
// 	{
// 		World->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
// 	}
//     
// 	Super::EndPlay(EndPlayReason);
// }
//
// void UOCMarkComponent::SetupGlobalASCEvents()
// {
// 	if (!GetWorld()) return;
// 	int32 ActorCount = 0;
// 	int32 ASCCount = 0;
//     
// 	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
// 	{
// 		ActorCount++;
// 		AActor* Actor = *ActorItr;
//         
// 		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
// 		if (ASC)
// 		{
// 			ASCCount++;
// 			UE_LOG(LogTemp, Warning, TEXT(" Binding to existing actor: %s"), *Actor->GetName());
//             
// 			FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(TEXT("State.Marked"));
//             
// 			ASC->RegisterGameplayTagEvent(MarkTag, EGameplayTagEventType::NewOrRemoved)
// 				.AddLambda([this, Actor](const FGameplayTag CallbackTag, int32 NewCount)
// 				{
// 					OnMarkTagChanged(CallbackTag, NewCount, Actor);
// 				});
// 		}
// 	}
//     
// 	UE_LOG(LogTemp, Warning, TEXT(" Processed %d actors, found %d with ASC"), ActorCount, ASCCount);
// }
//
// void UOCMarkComponent::OnActorSpawned(AActor* SpawnedActor)
// {
// 	if (!SpawnedActor) return;
//     
// 	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SpawnedActor);
// 	if (ASC)
// 	{
// 		FGameplayTag MarkTag = FGameplayTag::RequestGameplayTag(TEXT("State.Marked"));
//         
// 		ASC->RegisterGameplayTagEvent(MarkTag, EGameplayTagEventType::NewOrRemoved)
// 			.AddLambda([this, SpawnedActor](const FGameplayTag CallbackTag, int32 NewCount)
// 			{
// 				OnMarkTagChanged(CallbackTag, NewCount, SpawnedActor);
// 			});
//             
// 		UE_LOG(LogTemp, Log, TEXT("ASC event bound for: %s"), *SpawnedActor->GetName());
// 	}
// }
//
// void UOCMarkComponent::OnMarkTagChanged(const FGameplayTag CallbackTag, int32 NewCount, AActor* Actor)
// {
// 	if (!Actor || !IsValid(Actor)) return;
//
// 	if (NewCount > 0)
// 	{
// 		if (MarkVisualActorClass && !ActiveMarks.Contains(Actor))
// 		{
// 			FActorSpawnParameters SpawnParams;
// 			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
// 			AMarkVisualActor* MarkActor = GetWorld()->SpawnActor<AMarkVisualActor>(
// 				MarkVisualActorClass,
// 				Actor->GetActorLocation(),
// 				FRotator::ZeroRotator,
// 				SpawnParams
// 			);
//
// 			if (MarkActor)
// 			{
// 				MarkActor->SetTargetActor(Actor);
// 				ActiveMarks.Add(Actor, MarkActor);
//                 
// 				UE_LOG(LogTemp, Warning, TEXT("Auto mark created for: %s"), *Actor->GetName());
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if (TWeakObjectPtr<AMarkVisualActor>* FoundMark = ActiveMarks.Find(Actor))
// 		{
// 			if (FoundMark->IsValid())
// 			{
// 				(*FoundMark)->RemoveMark();
// 			}
// 			ActiveMarks.Remove(Actor);
//
// 			UE_LOG(LogTemp, Warning, TEXT("Auto mark removed for: %s"), *Actor->GetName());
// 		}
// 	}
// }
