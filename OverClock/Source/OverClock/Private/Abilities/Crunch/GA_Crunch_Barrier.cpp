#include "Abilities/Crunch/GA_Crunch_Barrier.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Data/GameDataType.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"
#include "Data/OCGameplayTags.h"

void UGA_Crunc_Barrier::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
										const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
										const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (Montage)
	{
		if (UAbilityTask_PlayMontageAndWait* Task = PlayMontageTask(Montage))
		{
			Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		}
	}

	if (IsServerAuthority())
	{
		ACharacter* C = GetOCCharacter();
		if (C!=nullptr && ProjectileClass != nullptr)
		{
			const FVector Loc = C->GetMesh() ? C->GetMesh()->GetSocketLocation(ThrowSocket) : C->GetActorLocation();
			const FRotator Rot = C->GetActorRotation();
			const FTransform XForm(Rot, Loc);

			FActorSpawnParameters SP;
			SP.Owner = C;
			SP.Instigator = C;

			GetWorld()->SpawnActor(ProjectileClass, &XForm, SP);
		}
	}
	
}

void UGA_Crunc_Barrier::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Crunc_Barrier::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
