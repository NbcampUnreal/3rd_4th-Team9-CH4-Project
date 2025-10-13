#include "Abilities/Crunch/GA_Crunch_RocketPunch.h"
#include "Abilities/OCGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Data/OCGameplayTags.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_Crunch_RocketPunch::UGA_Crunch_RocketPunch()
{
	AbilityTags.AddTag(OCGameplayTags::Ability_Mobility_Dash);
}

void UGA_Crunch_RocketPunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	AddLooseTag(OCGameplayTags::State_Mobility_Dashing);


	if (APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get()))
	{
		PC->SetIgnoreMoveInput(true);
	}
	
	if (Montage)
	{
		if (UAbilityTask_PlayMontageAndWait* Task = PlayMontageTask(Montage, 1.f))
		{
			Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		}

		if (ACharacter* C = GetOCCharacter())
		{
			if (UAnimInstance* Anim = C->GetMesh() ? C->GetMesh()->GetAnimInstance() : nullptr)
			{
				Anim->Montage_SetNextSection(FName("Start"), FName("Loop"), Montage);
			}
		}
	}
	
	if (IsServerAuthority())
	{
		BeginDash_Server();
	}
}

void UGA_Crunch_RocketPunch::OnMontageCompleted()
{
	RemoveLooseTag(OCGameplayTags::State_Mobility_Dashing);
	if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
	{
		if (APlayerController* PC = Cast<APlayerController>(Info->PlayerController.Get()))
		{
			PC->SetIgnoreMoveInput(false);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Crunch_RocketPunch::OnMontageInterrupted()
{
	RemoveLooseTag(OCGameplayTags::State_Mobility_Dashing);
	if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
	{
		if (APlayerController* PC = Cast<APlayerController>(Info->PlayerController.Get()))
		{
			PC->SetIgnoreMoveInput(false);
		}
	}
	
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(DashTimer);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Crunch_RocketPunch::BeginDash_Server()
{
	ACharacter* C = GetOCCharacter();
	CachedChar = C;
	if (!C)
	{
		FinishDash_Server(false);
		return;
	}

	StartLocation = C->GetActorLocation();
	DashDirection = C->GetActorForwardVector().GetSafeNormal();
	MoveDistance = 0.f;
	bDashFinished = false;

	if (UCharacterMovementComponent* MovementComponent = C->GetCharacterMovement())
	{
		MovementComponent->BrakingFrictionFactor = 0.f;
		MovementComponent->GroundFriction = 0.f;
	}

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(DashTimer, this, &ThisClass::TickDash_Server, StepInterval, true);
	}
}

void UGA_Crunch_RocketPunch::TickDash_Server()
{
	ACharacter* C = CachedChar.Get();
	if (!C || bDashFinished)
	{
		FinishDash_Server(false);
		return;
	}

	const float Steps = FMath::Max(1.f, DashDuration/StepInterval);
	const float StepDist = DashDistance / Steps;
	const FVector From = C->GetActorLocation();
	const FVector To = From + DashDirection * StepDist;

	FHitResult Hit;
	C->SetActorLocation(To, true, &Hit, ETeleportType::None);

	MoveDistance += StepDist;

	if (Hit.bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != C)
		{
			ApplyHitToActor_Server(HitActor, StartLocation, DashDirection);
			FinishDash_Server(true);
			return;
		}

		FinishDash_Server(false);
		return;
	}

	if (MoveDistance>=DashDistance)
	{
		FinishDash_Server(false);
		return;
	}
}

void UGA_Crunch_RocketPunch::FinishDash_Server(bool bHitSomething)
{
	if (bDashFinished) return;
	bDashFinished = true;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(DashTimer);
	}

	if (ACharacter* C = CachedChar.Get())
	{
		if (UCharacterMovementComponent* OCMovementComponent = C->GetCharacterMovement())
		{
			OCMovementComponent->BrakingFrictionFactor = 2.f;
			OCMovementComponent->GroundFriction = 8.f;
		}

		if (Montage)
		{
			if (UAnimInstance* Anim = C->GetMesh() ? C->GetMesh()->GetAnimInstance() : nullptr)
			{
				if (Anim->Montage_IsPlaying(Montage))
				{
					Anim->Montage_SetNextSection(FName("Loop"), FName("End"), Montage);
				}
			}
		}
	}
}



void UGA_Crunch_RocketPunch::ApplyHitToActor_Server(AActor* HitActor, const FVector& From, const FVector& Forward) const
{
	if (DamageGE)
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (UAbilitySystemComponent* TargetASC =
				UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor))
			{
				FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
				Ctx.AddSourceObject(GetAvatarActorFromActorInfo());
				FGameplayEffectSpecHandle   Spec = SourceASC->MakeOutgoingSpec(DamageGE, GetAbilityLevel(), Ctx);
				if (Spec.IsValid())
				{
					SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
				}
			}
		}
	}
	
	const FVector Push = Forward * KnockbackStrength + FVector(0,0,KnockbackUp);
	if (ACharacter* VictimChar = Cast<ACharacter>(HitActor))
	{
		VictimChar->LaunchCharacter(Push, true, true);
	}
	else if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(HitActor->GetRootComponent()))
	{
		if (Prim->IsSimulatingPhysics())
		{
			Prim->AddImpulse(Push, NAME_None, true);
		}
	}
}
