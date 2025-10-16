#include "Abilities/Revenant/GA_DeadlyBullet.h"

#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/OCRBMissile.h"
#include "Data/OCGameplayTags.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

UGA_DeadlyBullet::UGA_DeadlyBullet()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	bServerRespectsRemoteAbilityCancellation = false;

	AbilityTags.AddTag(OCGameplayTags::Ability_DeadlyBullet);
}

void UGA_DeadlyBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	if (MarkMontage)
	{
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, MarkMontage);

		Task->OnCompleted.AddDynamic(this, &UGA_DeadlyBullet::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_DeadlyBullet::OnMontageCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_DeadlyBullet::OnMontageCancelled);
		
		Task->ReadyForActivation();
	}
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 가장 가까운 적
		AActor* Target = FindNearestEnemy();
		if (!Target)
		{
			UE_LOG(LogTemp, Warning, TEXT("No target found for Homing Mark"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		if (HasAuthority(&ActivationInfo))
		{
			LaunchHomingProjectile(Target);
		}
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
	float NearestDistance = FLT_MAX;
	FVector MyLocation = Avatar->GetActorLocation();

	for (AActor* Actor : FoundActors)
	{
		if (Actor == Avatar) continue;
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor);
		if (!ASI) continue;
		UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
		if (!TargetASC) continue;
		
		//같은 태그인지 확인 필요
		//if(AvatarASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Red") == TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Red")) 
		//{
		//continue;
		//}

		float Distance = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
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
		UE_LOG(LogTemp, Error, TEXT("[UGA_DeadlyBullet] MissileClass or Target Is Null"));
		return;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 200.0f;
	SpawnLocation.Z+=150.f;
	FVector DirectionToTarget = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AOCRBMissile* Projectile = GetWorld()->SpawnActor<AOCRBMissile>(
		MissileClass, 
		SpawnLocation, 
		DirectionToTarget.Rotation(),
		SpawnParams
	))
	{
		Projectile->SetTarget(Target);
		//UE_LOG(LogTemp, Warning, TEXT("[UGA_DeadlyBullet] Missile Spawned"));
	}
}

void UGA_DeadlyBullet::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DeadlyBullet::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}