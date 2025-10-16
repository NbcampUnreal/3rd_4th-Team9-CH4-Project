#include "Component/OCMarkComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UOCMarkComponent::UOCMarkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	SetVisibility(false);
	SetRelativeLocation(MarkOffset);
	SetRelativeRotation(FRotator(180, 0, 0));
}

void UOCMarkComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOCMarkComponent::ASCBind()
{
	// Component의 Owner ASC에 이벤트 등록
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerPawn))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				// Owner ASC에 MarkTag 변경 이벤트 바인딩
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

void UOCMarkComponent::Multicast_SetMarkVisibility_Implementation(bool InVis)
{
	if(InVis)
	{
		SetVisibility(true);
	}
	else
	{
		SetVisibility(false);
	}
}
