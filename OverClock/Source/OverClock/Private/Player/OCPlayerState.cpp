#include "Player/OCPlayerState.h"

#include "AbilitySystemComponent.h"

AOCPlayerState::AOCPlayerState()
{
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AOCPlayerState::BeginPlay()
{
	Super::BeginPlay();
	// 필요 시 시작 효과/스탯 부여 가능, 아바타 연결 후에도 가능하니까 알아볼것!
}

UAbilitySystemComponent* AOCPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOCPlayerState::InitASCForAvatar(AActor* NewAvatar)
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, NewAvatar);

		if (HasAuthority() && !bGrantedOnce)
		{
			/*
			 * To Do : 시작 능력/효과 부여 자리에 사용(필요시)
			 * AbilitySystemComponent->GiveAbility(파라미터들)
			 * AbilitySystemComponent->ApplyGameplayEffectToSelf(파라미터들)
			 */
			bGrantedOnce = true;
		}
	}
}

