// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OCGemMissile.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AOCGemMissile::AOCGemMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	InitialSpeed = 2000.0f;
	MaxSpeed = 2500.0f;
	LifeSpan = 5.0f;

	NiagaraComp=CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->SetCustomTimeDilation(5.0f);
	NiagaraComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 충돌 설정
	CollisionComponent->SetSphereRadius(20.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->ProjectileGravityScale=0.0f;
}

void AOCGemMissile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AOCGemMissile::OnOverlapBegin);
}

void AOCGemMissile::BeginPlay()
{
	Super::BeginPlay();
}

void AOCGemMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bInitial) return;

	static float TotalYaw = 0.0f;
	TotalYaw += DeltaTime * 180.0f;

	SetActorRotation(FRotator(0, TotalYaw, 0));
}

void AOCGemMissile::Init()
{
	Super::Init();

	if (!bInitial) return;
	
	// TArray<AActor*> AllWorldActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllWorldActors);
	// FGameplayTag OwnerTag = GetTeamTag(GetOwner());
	// for (AActor* Actor : AllWorldActors)
	// {
	// 	if (Actor)
	// 	{
	// 		if (GetTeamTag(Actor) != OwnerTag) // 다른 team이면 ignore
	// 		{
	// 			CollisionComponent->IgnoreActorWhenMoving(Actor, true);
	// 		}
	// 	}
	// }
	GetWorld()->GetTimerManager().SetTimer(EndTimerHandle, this, 
		&AOCGemMissile::UnInit, 5.0f, false);
}

void AOCGemMissile::UnInit()
{
	Super::UnInit();

	if (EndTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(EndTimerHandle);
	}
}

void AOCGemMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (OtherActor->GetLocalRole()==ROLE_AutonomousProxy)
	// {
	// 	return;
	// }
	if (/*!HasAuthority() ||*/ !GameEffectClass || !OtherActor) return;

	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		// Heal Sound
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, SweepResult.Location);
		}
		
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
		if (!ASI) return;
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		if (!ASC) return;
		// Heal GE
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		ASC->ApplyGameplayEffectToSelf(GameEffectClass.GetDefaultObject(), 1, EffectContext);
        
		UnInit();
	}
}

void AOCGemMissile::SetVelocity(const FVector& NewVelocity)
{
	ProjectileMovement->Velocity = NewVelocity*InitialSpeed;
}
