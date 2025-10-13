#include "Abilities/Crunch/GA_CrunchBase.h"

#include "Abilities/OCAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_CrunchBase::UGA_CrunchBase()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_CrunchBase::BoxTraceAndApplyToTargets_Server(const FVector& Start, const FVector& End,
	const FVector& HalfExtent, TSubclassOf<UGameplayEffect> GE, bool bStopAtFirstHit) const
{
	if (!IsServerAuthority() || !GE) return;
	ACharacter* C = GetOCCharacter();
	if (C==nullptr) return;

	TArray<FHitResult> Hits;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(C);
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Start, End, HalfExtent, C->GetActorRotation(), UEngineTypes::ConvertToTraceType(ECC_Pawn), false, IgnoreActors, EDrawDebugTrace::None, Hits, true);

	for (int32 Index = 0; Index < Hits.Num(); ++Index)
	{
		const FHitResult& Hit = Hits[Index];
		UOCAbilitySystemComponent* TargetASC = GetTargetOCASC(Hit.GetActor());
		if (TargetASC != nullptr)
		{
			UOCAbilitySystemComponent* OCASC = GetOCASC();
			if (OCASC != nullptr)
			{
				FGameplayEffectContextHandle Context = OCASC->MakeEffectContext();
				Context.AddHitResult(Hit);
				TargetASC->ApplyGameplayEffectToSelf(GE->GetDefaultObject<UGameplayEffect>(), 1.f, Context);
			}
			if (bStopAtFirstHit) break;
		}
	}
}

void UGA_CrunchBase::SphereTraceAndApplyToTargets_Server(const FVector& Center, float Radius,
	TSubclassOf<UGameplayEffect> DamageGE, TSubclassOf<UGameplayEffect> CcGE) const
{
	if (!IsServerAuthority() || (!DamageGE && !CcGE)) return;
	ACharacter* C = GetOCCharacter();
	if (C == nullptr) return;

	TArray<FHitResult> Hits;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(C);
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Center, Center, Radius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, IgnoreActors, EDrawDebugTrace::None, Hits, true);
	for (int32 Index = 0; Index < Hits.Num(); ++Index)
	{
		const FHitResult& Hit = Hits[Index];
		UOCAbilitySystemComponent* TargetASC = GetTargetOCASC(Hit.GetActor());
		if (TargetASC != nullptr)
		{
			UOCAbilitySystemComponent* OCASC = GetOCASC();
			if (OCASC == nullptr) continue;

			FGameplayEffectContextHandle Context = OCASC->MakeEffectContext();
			Context.AddHitResult(Hit);
			if (DamageGE)
			{
				TargetASC->ApplyGameplayEffectToSelf(DamageGE->GetDefaultObject<UGameplayEffect>(), 1.f, Context);
			}
			if (CcGE)
			{
				TargetASC->ApplyGameplayEffectToSelf(CcGE->GetDefaultObject<UGameplayEffect>(), 1.f, Context);
			}	
		}
	}
}

void UGA_CrunchBase::CapsuleSweepAndApplyToTargets_Server(const FVector& Start, const FVector& End, float Radius,
	float HalfHeight, TSubclassOf<UGameplayEffect> GE) const
{
	if (!IsServerAuthority() || !GE) return;
	ACharacter* C = GetOCCharacter();
	if (C == nullptr) return;

	TArray<FHitResult> Hits;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(C);
	UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), Start, End, Radius, HalfHeight, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, IgnoreActors, EDrawDebugTrace::None, Hits, true);

	UOCAbilitySystemComponent* OCASC = GetOCASC();
	if (OCASC == nullptr) return;

	for (int32 Index = 0; Index < Hits.Num(); ++Index)
	{
		const FHitResult& Hit = Hits[Index];
		UOCAbilitySystemComponent* TargetASC = GetTargetOCASC(Hit.GetActor());
		if (TargetASC != nullptr)
		{
			FGameplayEffectContextHandle Context = OCASC->MakeEffectContext();
			Context.AddHitResult(Hit);
			TargetASC->ApplyGameplayEffectToSelf(GE->GetDefaultObject<UGameplayEffect>(), 1.f, Context);
		}
	}
}

UOCAbilitySystemComponent* UGA_CrunchBase::GetTargetOCASC(const AActor* Target) const
{
	if (Target == nullptr) return nullptr;

	UAbilitySystemComponent* BaseASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true);

	return Cast<UOCAbilitySystemComponent>(BaseASC);
}



