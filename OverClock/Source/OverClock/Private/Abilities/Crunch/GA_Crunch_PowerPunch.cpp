#include "Abilities/Crunch/GA_Crunch_PowerPunch.h"
#include "Data/OCGameplayTags.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "TimerManager.h"

void UGA_Crunch_PowerPunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Log, TEXT("PowerPunch Activate"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	bInputHold = true;
	AddLooseTag(OCGameplayTags::State_Weapon_Firing);
	CurrentComboIndex = FMath::Clamp(CurrentComboIndex, 0, ComboMontages.Num()- 1);
	StartStep();
	
}

void UGA_Crunch_PowerPunch::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	bInputHold = true;
	
	if (bWindowOpen)
	{
		bQueued = true;
	}
}

void UGA_Crunch_PowerPunch::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	bInputHold = false;
}

void UGA_Crunch_PowerPunch::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearAllTimers();
	bInputHold = false;
	RemoveLooseTag(OCGameplayTags::State_Weapon_Firing);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Crunch_PowerPunch::StartStep()
{
	UE_LOG(LogTemp, Log, TEXT("StartStep idx=%d, Montage=%s"), CurrentComboIndex, *GetNameSafe(ComboMontages[CurrentComboIndex]));
	bQueued = false;

	if (ComboMontages.IsValidIndex(CurrentComboIndex))
	{
		UAnimMontage* Montage = ComboMontages[CurrentComboIndex];
		if (Montage)
		{
			UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, 1.f, NAME_None, true, 1.f, 0.f, false);
			if (Task)
			{
				UE_LOG(LogTemp, Log, TEXT("PowerPunch: Play montage %s"), *GetNameSafe(Montage));
				Task->ReadyForActivation();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PowerPunch: Failed to create PlayMontage task"));
			}
		}
	}

	const float OpenAt = ComboWindowOpens.IsValidIndex(CurrentComboIndex) ? ComboWindowOpens[CurrentComboIndex] : -1.f;
	const float CloseAt = ComboWindowCloses.IsValidIndex(CurrentComboIndex) ? ComboWindowCloses[CurrentComboIndex] : -1.f;

	if (OpenAt >= 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TH_Open, this, &UGA_Crunch_PowerPunch::OpenWindow, OpenAt, false);
	}
	if (CloseAt >= 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TH_Close, this, &UGA_Crunch_PowerPunch::CloseWindow, CloseAt, false);
	}

	const float HitAt = (OpenAt >= 0.f ? OpenAt : 0.2f) + 0.05f;
	GetWorld()->GetTimerManager().SetTimer(TH_Hit, this, &UGA_Crunch_PowerPunch::DoHit_Server, HitAt, false);

	const float EndAt = (CloseAt >  0.f ? CloseAt : 0.4f) + 0.1f;
	GetWorld()->GetTimerManager().SetTimer(TH_End, this, &UGA_Crunch_PowerPunch::OnStepEnd, EndAt, false);
}

void UGA_Crunch_PowerPunch::OpenWindow()
{
	bWindowOpen = true;
	if (bInputHold)
	{
		bQueued = true;
	}
}

void UGA_Crunch_PowerPunch::CloseWindow()
{
	bWindowOpen = false;
}

void UGA_Crunch_PowerPunch::DoHit_Server()
{
	if (!IsServerAuthority() || DamageGE == nullptr) return;

	ACharacter* C = GetOCCharacter();
	if (C == nullptr) return;

	const FVector Start = C->GetActorLocation() + C->GetActorForwardVector() * 100.f + FVector(0.f, 0.f, 50.f);
	const FVector End = Start + C->GetActorForwardVector() * PunchRange;

	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, /*bPersistentLines=*/false, /*LifeTime=*/1.f, 0, 2.f);
	DrawDebugBox(GetWorld(), Start, PunchHalfExtent, C->GetActorQuat(), FColor::Green, /*bPersistentLines=*/false, /*LifeTime=*/1.f);
	DrawDebugBox(GetWorld(), End,   PunchHalfExtent, C->GetActorQuat(), FColor::Red,   /*bPersistentLines=*/false, /*LifeTime=*/1.f);

	BoxTraceAndApplyToTargets_Server(Start, End, PunchHalfExtent, DamageGE, true);
}

void UGA_Crunch_PowerPunch::OnStepEnd()
{
	if (bInputHold)
	{
		const int32 Num = ComboMontages.Num();
		if (Num > 0)
		{
			CurrentComboIndex = (CurrentComboIndex + 1) % Num;
			StartStep();
			return;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TH_Reset, [this]()
	{
		CurrentComboIndex = 0;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}, ComboResetTime, false);
}

void UGA_Crunch_PowerPunch::ClearAllTimers()
{
	FTimerManager& TM = GetWorld()->GetTimerManager();
	TM.ClearTimer(TH_Open);
	TM.ClearTimer(TH_Close);
	TM.ClearTimer(TH_Hit);
	TM.ClearTimer(TH_End);
	TM.ClearTimer(TH_Reset);
}
