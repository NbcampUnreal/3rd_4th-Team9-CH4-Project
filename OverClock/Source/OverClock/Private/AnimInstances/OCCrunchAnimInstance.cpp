#include "AnimInstances/OCCrunchAnimInstance.h"
#include "Player/OCCharacterBase_V2.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Data/OCGameplayTags.h"

void UOCCrunchAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UOCCrunchAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	UpdateGASState();
}

void UOCCrunchAnimInstance::UpdateGASState()
{
	if (!OwningCharacter) return;

	UAbilitySystemComponent* ASC = OwningCharacter->GetAbilitySystemComponent();
	if (!ASC) return;
	
	bIsAttacking    = ASC->HasMatchingGameplayTag(OCGameplayTags::State_Weapon_Firing);
	bIsUsingBarrier = ASC->HasMatchingGameplayTag(OCGameplayTags::State_Defense_Blocking);
	bIsDashing      = ASC->HasMatchingGameplayTag(OCGameplayTags::State_Mobility_Dashing);
	bIsUltimate     = ASC->HasMatchingGameplayTag(OCGameplayTags::State_Ultimate_Active);
}