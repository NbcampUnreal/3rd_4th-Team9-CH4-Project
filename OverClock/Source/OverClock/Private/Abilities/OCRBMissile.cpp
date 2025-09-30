#include "Abilities/OCRBMissile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameEffects/GE_MarkEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

AOCRBMissile::AOCRBMissile()
{
	PrimaryActorTick.bCanEverTick = false;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;
	

    // 충돌 컴포넌트 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    RootComponent = CollisionComponent;

    // 메시 컴포넌트
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 발사체 이동 컴포넌트
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->BeginReplication();
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;//충돌하면 멈추는가 튕기는가
    ProjectileMovement->bInterpMovement=true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;

    // 트레일 이펙트
    TrailEffect = CreateDefaultSubobject<UNiagaraSystem>(TEXT("TrailEffect"));
	
    // 수명 설정
    InitialLifeSpan = LifeSpan;

    // 충돌 이벤트 바인딩
    CollisionComponent->OnComponentHit.AddDynamic(this, &AOCRBMissile::OnHit);
}

void AOCRBMissile::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true); //위치 && 회전 복제
	//UE_LOG(LogTemp, Warning, TEXT("[AOCRBMissile BeginPlay] %s, "), Net)

	// 발사 사운드 재생
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
}

void AOCRBMissile::SetTarget(AActor* NewTarget)
{
	if (IsValid(NewTarget))
	{
		HomingTarget = NewTarget;
		NewTargetInTick = HomingTarget;
		ProjectileMovement->HomingTargetComponent = NewTarget->GetRootComponent();
		ProjectileMovement->bIsHomingProjectile = true;
        
		UE_LOG(LogTemp, Log, TEXT("Homing target set to: %s"), *NewTarget->GetName());
	}
	else
	{
		HomingTarget = nullptr;
		ProjectileMovement->HomingTargetComponent = nullptr;
		ProjectileMovement->bIsHomingProjectile = false;
        
		UE_LOG(LogTemp, Warning, TEXT("Invalid homing target provided"));
	}
}

void AOCRBMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//if (HasAuthority())
	{
		// 자기 자신이나 발사자는 제외
		if (OtherActor && OtherActor->IsA(APawn::StaticClass()) && OtherActor != GetOwner())
		{
			AActor* OwnerActor = GetOwner();//Debug 확인용
			DrawDebugSphere(GetWorld(), GetActorLocation(),50.0f, 12, FColor::Red, false, 10.0f, 0, 5.0f);
	        	        
			// 히트 사운드 재생
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location);
			}

			// 표식 적용
			ApplyMarkToTarget(OtherActor);
	        
			// 발사체 제거
			Destroy();
		}
	}
}

void AOCRBMissile::ApplyMarkToTarget(AActor* Target)
{
	if (!HasAuthority() || !MarkEffectClass || !Target) return;
    
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	// 발사자(Owner)의 ASC 사용
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!OwnerASC) return;

	FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(MarkEffectClass, 1, EffectContext);
	
	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		Spec->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Marked")));
		OwnerASC->ApplyGameplayEffectSpecToTarget(*Spec, TargetASC);
	}
}

//delete
void AOCRBMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
