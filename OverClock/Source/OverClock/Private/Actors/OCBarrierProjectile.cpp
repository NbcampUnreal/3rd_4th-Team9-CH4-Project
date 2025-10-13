#include "Actors/OCBarrierProjectile.h"
#include "Actors/OCBarrierField.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AOCBarrierProjectile::AOCBarrierProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(12.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 1200.f;
	Movement->MaxSpeed = 1200.f;
	Movement->ProjectileGravityScale = 1.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	Movement->OnProjectileStop.AddDynamic(this, &AOCBarrierProjectile::OnProjectileStop);
}

void AOCBarrierProjectile::Init(AController* InOwnerController, TSubclassOf<AOCBarrierField> InBarrierClass,
	float InBarrierRadius, float InBarrierHalfHeight, float InBarrierLifeSeconds, float InBarrierMaxHealth)
{
	OwnerController = InOwnerController;
	BarrierClass = InBarrierClass;
	BarrierRadius = InBarrierRadius;
	BarrierHalfHeight = InBarrierHalfHeight;
	BarrierLifeSeconds = InBarrierLifeSeconds;
	BarrierMaxHealth = InBarrierMaxHealth;
}

void AOCBarrierProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AOCBarrierProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOCBarrierProjectile, OwnerController);
}

void AOCBarrierProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (!HasAuthority()) return;
	if (!BarrierClass)
	{
		Destroy();
		return;
	}

	const FVector SpawnLocation = ImpactResult.Location + FVector(0.f, 0.f, 2.f);
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AOCBarrierField* Barrier = GetWorld()->SpawnActor<AOCBarrierField>(BarrierClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Barrier)
	{
		Barrier->InitOwnerController(OwnerController);
		Barrier->InitParams(BarrierRadius, BarrierHalfHeight, BarrierLifeSeconds, BarrierMaxHealth);
	}
	
	Destroy();
}



