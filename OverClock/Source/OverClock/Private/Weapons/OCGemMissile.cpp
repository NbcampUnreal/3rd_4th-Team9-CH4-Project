// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OCGemMissile.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
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

	// 충돌 설정
	CollisionComponent->SetSphereRadius(20.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->ProjectileGravityScale=0.0f;
}

void AOCGemMissile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    CollisionComponent->OnComponentHit.AddDynamic(this, &AOCGemMissile::OnHit);
}

void AOCGemMissile::BeginPlay()
{
	Super::BeginPlay();
}

void AOCGemMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bInitial) return;
	AddActorLocalRotation(FRotator(0, 0, DeltaTime * 180.0f));
	UE_LOG(LogTemp, Warning, TEXT("%f"),GetActorRotation().Roll)
}

void AOCGemMissile::Init()
{
	Super::Init();
	if (!bInitial) return;
	
	TArray<AActor*> AllWorldActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllWorldActors);
	FGameplayTag OwnerTag = GetTeamTag(GetOwner());
	for (AActor* Actor : AllWorldActors)
	{
		if (Actor)
		{
			if (GetTeamTag(Actor) != OwnerTag) // 다른 team이면 ignore
			{
				CollisionComponent->IgnoreActorWhenMoving(Actor, true);
			}
		}
	}
}

void AOCGemMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	if (!HasAuthority() || !GameEffectClass || !OtherActor) return;

	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		//같은 팀인지 확인 > 중복 확인
		if (GetTeamTag(OtherActor)!=GetTeamTag(GetOwner())) return;

		UAbilitySystemComponent* TargetASC=  GetASC(OtherActor);

		// Heal Sound
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location);
		}
        
		// Heal GE
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		TargetASC->ApplyGameplayEffectToSelf(GameEffectClass.GetDefaultObject(), 1, EffectContext);
        
		UnInit();
	}
}

void AOCGemMissile::SetVelocity(const FVector& NewVelocity)
{
	ProjectileMovement->Velocity = NewVelocity*InitialSpeed;
}
