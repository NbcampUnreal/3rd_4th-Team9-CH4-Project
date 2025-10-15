#include "Weapons/OCWhipMissile.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"//사운드 변경 후 삭제

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
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->ProjectileGravityScale=0.0f;
}

void AOCWhipMissile::Init()
{
	Super::Init();
	//TimerHandle 초기화
	
	if (!bInitial) return;
	
	TArray<AActor*> AllWorldActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllWorldActors);
	FGameplayTag OwnerTag = GetTeamTag(GetOwner());
	for (AActor* Actor : AllWorldActors)
	{
		if (Actor)
		{
			if (GetTeamTag(Actor) == OwnerTag) // 같은 team이면 ignore
			{
				CollisionComponent->IgnoreActorWhenMoving(Actor, true);
			}
		}
	}
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
    CollisionComponent->OnComponentHit.AddDynamic(this, &AOCWhipMissile::OnHit);
}

void AOCWhipMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	if (OtherActor==GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AOCWhipMissile] OtherActor Is Owner"))
		return;
	}

    if (!HasAuthority() || !GameEffectClass || !OtherActor) return;

    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        if (GetTeamTag(OtherActor)==GetTeamTag(GetOwner())) return;
		
        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location);
        }

        // Damage GE 적용
    	UAbilitySystemComponent* TargetASC = GetASC(OtherActor);
    	if (!TargetASC) return;
        FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
    	TargetASC->ApplyGameplayEffectToSelf(GameEffectClass.GetDefaultObject(), 1, EffectContext);
        
        UnInit();
    }
}

void AOCWhipMissile::SetVelocity(const FVector& NewVelocity)
{
	ProjectileMovement->Velocity = NewVelocity*InitialSpeed;
}
