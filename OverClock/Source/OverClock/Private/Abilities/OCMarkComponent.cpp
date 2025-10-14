#include "Abilities/OCMarkComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UOCMarkComponent::UOCMarkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	SetVisibility(false); // 기본적으로 숨김
	SetRelativeLocation(MarkOffset);
	SetRelativeRotation(FRotator(180, 0, 0));

	// 파티클 효과들
	//MarkEffect = CreateDefaultSubobject<UNiagaraSystem>(TEXT("MarkEffect"));
}

void UOCMarkComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOCMarkComponent::ASCBind()
{
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
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerPawn))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Marked")))
				{
					Multicast_SetMarkVisibility(true);
				}
				else
				{
					Multicast_SetMarkVisibility(false);
				}
			}
		}
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
	// }
}

void UOCMarkComponent::Multicast_SetMarkVisibility_Implementation(bool InVis) //서버는 안 하도록 변경
{
	if(InVis)
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
