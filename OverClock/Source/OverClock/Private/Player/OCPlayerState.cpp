#include "Player/OCPlayerState.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Util/UtilTeam.h"
#include "Abilities/Attributes/OCAttributeSet_Health.h"
#include "Abilities/Attributes/OCAttributeSet_Resources.h"

AOCPlayerState::AOCPlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UOCAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthAttributeSet = CreateDefaultSubobject<UOCAttributeSet_Health>(TEXT("Health AttributeSet"));
	ResourcesAttributeSet =	CreateDefaultSubobject<UOCAttributeSet_Resources>(TEXT("Resources AttributeSet"));

	AbilitySystemComponent->AddAttributeSetSubobject(HealthAttributeSet.Get());
	AbilitySystemComponent->AddAttributeSetSubobject(ResourcesAttributeSet.Get());
}

void AOCPlayerState::BeginPlay()
{
	Super::BeginPlay();
	// 필요 시 시작 효과/스탯 부여 가능, 아바타 연결 후에도 가능하니까 알아볼것!
}

UAbilitySystemComponent* AOCPlayerState::GetAbilitySystemComponent() const
{
	return GetOCASC();
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

void AOCPlayerState::SetInfo(int32 InTeamId)
{
	if (nullptr == UtilTeamInfo)
	{
		UtilTeamInfo = NewObject<UUtilTeam>(this);
	}

	UtilTeamInfo->Initialize(InTeamId);
}

UUtilTeam* AOCPlayerState::GetInfo() const
{
	return UtilTeamInfo;
}

