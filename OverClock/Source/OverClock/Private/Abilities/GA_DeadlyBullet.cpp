#include "Abilities/GA_DeadlyBullet.h"

#include "GameplayEffect.h"
#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "Abilities/OCMarkComponent.h"
#include "Abilities/OCRBMissile.h"
#include "Data/OCGameplayTags.h"
#include "GameEffects/GE_MarkEffect.h"

UGA_DeadlyBullet::UGA_DeadlyBullet()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bServerRespectsRemoteAbilityCancellation = false;

	AbilityTags.AddTag(OCGameplayTags::Ability_DeadlyBullet);
}

void UGA_DeadlyBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}

		// 가장 가까운 적
		AActor* Target = FindNearestEnemy();
		if (!Target)
		{
			UE_LOG(LogTemp, Warning, TEXT("No target found for Homing Mark"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		LaunchHomingProjectile(Target);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_DeadlyBullet::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

AActor* UGA_DeadlyBullet::FindNearestEnemy()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return nullptr;

	TArray<AActor*> FoundActors;
    
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), FoundActors);

	AActor* NearestEnemy = nullptr;
	float NearestDistance = TargetRange;
	FVector MyLocation = Avatar->GetActorLocation();

	for (AActor* Actor : FoundActors)
	{
		if (Actor == Avatar) continue;
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
		if (!TargetASC) continue;
        
		// 적 태그 확인
		//if (!TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Enemy"))))
		//	continue;

		float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestEnemy = Actor;
		}
	}

	return NearestEnemy;
}

void UGA_DeadlyBullet::LaunchHomingProjectile(AActor* Target)
{
	if (!MissileClass || !Target)
	{
		UE_LOG(LogTemp, Error, TEXT("MissileClass is null! Check if the C++ class is being used directly instead of the Blueprint."));
		return;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 200.0f;
	SpawnLocation.Z=300.f;
	FVector DirectionToTarget = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AOCRBMissile* Projectile = GetWorld()->SpawnActor<AOCRBMissile>(
		MissileClass, 
		SpawnLocation, 
		DirectionToTarget.Rotation(),
		SpawnParams
	);

	if (Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("MissileClass Name : %s"),*MissileClass->GetName());
		Projectile->SetTarget(Target);
	}
}