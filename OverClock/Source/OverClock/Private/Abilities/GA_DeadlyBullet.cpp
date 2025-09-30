#include "Abilities/GA_DeadlyBullet.h"

#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Abilities/OCRBMissile.h"
#include "Data/OCGameplayTags.h"

UGA_DeadlyBullet::UGA_DeadlyBullet()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;//server만

	bServerRespectsRemoteAbilityCancellation = false;//클라 취소 불가

	AbilityTags.AddTag(OCGameplayTags::Ability_DeadlyBullet);//ASC에 GA부여
}

void UGA_DeadlyBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//network 확인용
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		ENetMode NetMode = ActorInfo->AvatarActor->GetNetMode();
		UE_LOG(LogTemp, Warning, TEXT("[UGA_DeadlyBullet] DedicatedServer : 1 | Client : 3 | Now : %d"), NetMode)
	}
	
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
		
		if (HasAuthority(&ActivationInfo))
		{
			//network 확인용
			if (ActorInfo && ActorInfo->AvatarActor.IsValid())
			{
				ENetMode NetMode = ActorInfo->AvatarActor->GetNetMode();
				UE_LOG(LogTemp, Warning, TEXT("[UGA_DeadlyBullet] DedicatedServer : 1 | Client : 3 | Now : %d"), NetMode)
			}
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
		
		//같은 태그인지 확인
		//Tag1 = AvatarASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Red"))
		//Tag2 = TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Red"))
		//if(Tag1 = Tag2) 
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
		UE_LOG(LogTemp, Warning, TEXT("[UGA_DeadlyBullet] Missile Spawned"));
	}
}