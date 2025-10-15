#include "Weapons/OCWhipMissile.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"//사운드 변경 후 삭제
#include "ProfilingDebugging/CookStats.h"

AOCWhipMissile::AOCWhipMissile()
{
	PrimaryActorTick.bCanEverTick = false;

    InitialSpeed = 2000.0f;
    MaxSpeed = 2500.0f;
    LifeSpan = 5.0f;

    // 충돌 설정
    CollisionComponent->SetSphereRadius(20.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->ProjectileGravityScale=0.0f;
}

void AOCWhipMissile::Init()
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
	// 		if (GetTeamTag(Actor) == OwnerTag) // 같은 team이면 ignore
	// 		{
	// 			CollisionComponent->IgnoreActorWhenMoving(Actor, true);
	// 		}
	// 	}
	// }
	
	GetWorld()->GetTimerManager().SetTimer(EndTimerHandle, this, 
	&AOCWhipMissile::UnInit, 5.0f, false);
}

void AOCWhipMissile::UnInit()
{
	Super::UnInit();
	
	if (EndTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(EndTimerHandle);
	}
}

void AOCWhipMissile::BeginPlay()
{
	Super::BeginPlay();
}

void AOCWhipMissile::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AOCWhipMissile::OnOverlapBegin);
}

void AOCWhipMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, HitSound, SweepResult.Location);
        }

        // Damage GE 적용
    	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
    	if (!ASI) return;
    	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
    	if (!ASC) return;
        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    	ASC->ApplyGameplayEffectToSelf(GameEffectClass.GetDefaultObject(), 1, EffectContext);
        
        UnInit();
    }
}

void AOCWhipMissile::SetVelocity(const FVector& NewVelocity)
{
	ProjectileMovement->Velocity = NewVelocity*InitialSpeed;
}
