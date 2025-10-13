#include "Weapons/OCMissileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
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
}

FGameplayTag AOCMissileBase::GetTeamTag(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = GetASC(TargetActor);
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AOCMissileBase] TargetASC Is Null"))
		return FGameplayTag();
	}
	
	FGameplayTagContainer ActorTags;
	TargetASC->GetOwnedGameplayTags(ActorTags);

	for (const FGameplayTag& Tag : ActorTags)
	{
		if (Tag.ToString().StartsWith(TEXT("Team.")))
			return Tag;
	}
	return FGameplayTag();
}

TObjectPtr<UAbilitySystemComponent> AOCMissileBase::GetASC(AActor* TargetActor)
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor);
	if (!ASI) return nullptr;
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return nullptr;
	return ASC;
}

void AOCMissileBase::Alloc()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bInitial = true;
}

void AOCMissileBase::Init()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bInitial=false;
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
}

void AOCMissileBase::UnInit()
{
	bInitial=true;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorLocation(FVector::ZeroVector);
	SetActorTickEnabled(false);
	ProjectileMovement->Velocity = FVector::ZeroVector;
}

void AOCMissileBase::Release()
{
	Destroy();
}
