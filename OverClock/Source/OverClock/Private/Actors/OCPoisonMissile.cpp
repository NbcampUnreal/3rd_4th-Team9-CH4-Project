#include "Actors/OCPoisonMissile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/OCPoisonField.h"

AOCPoisonMissile::AOCPoisonMissile()
{
	PrimaryActorTick.bCanEverTick = false;

	InitialSpeed = 3000.0f;
	MaxSpeed = 3000.0f;
	LifeSpan = 5.0f;

	// 충돌 설정
	CollisionComponent->SetSphereRadius(20.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 바닥
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);        // 캐릭터
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);

	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->ProjectileGravityScale=0.1f;//중력
}

void AOCPoisonMissile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    CollisionComponent->OnComponentHit.AddDynamic(this, &AOCPoisonMissile::OnHit);
	if (GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}
	// TArray<AActor*> AllWorldActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllWorldActors);
	// FGameplayTag OwnerTag = GetTeamTag(GetOwner());
	// for (AActor* Actor : AllWorldActors)
	// {
	// 	if (Actor && Actor!=GetOwner())
	// 	{
	// 		if (GetTeamTag(Actor) == OwnerTag)
	// 		{
	// 			CollisionComponent->IgnoreActorWhenMoving(Actor, true);
	// 		}
	// 	}
	// }
}

void AOCPoisonMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	//ProjectileMovement->Velocity = InitialSpeed;
	if (!OtherActor || OtherActor == GetOwner()) return;
	
	// 장판 생성 함수 호출 후 삭제
	if (PoisonFieldClass)
	{
		FActorSpawnParameters Params;
        Params.Owner = GetOwner();
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        if (GetWorld()->SpawnActor<AOCPoisonField>(
        		PoisonFieldClass,
        		Hit.ImpactPoint,
        		FRotator::ZeroRotator,
        		Params
        	))
        {
        	if (!HitSound) return;
        	UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location);
        }
		ProjectileMovement->StopMovementImmediately();
	}
	Destroy();
}

void AOCPoisonMissile::SetVelocity(const FVector& NewVelocity)
{
	ProjectileMovement->Velocity = NewVelocity*InitialSpeed;
}
