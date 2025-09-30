#include "Abilities/OCMissileBase.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AOCMissileBase::AOCMissileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	// 충돌 컴포넌트
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = CollisionComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->BeginReplication();
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;//충돌하면 멈춤
	ProjectileMovement->bInterpMovement=true;

	// 메시 컴포넌트
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 수명 설정
	InitialLifeSpan = LifeSpan;
}

void AOCMissileBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true); //위치 && 회전 복제

	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
}

void AOCMissileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
}
