#include "Abilities/OCRBMissile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameEffects/GE_MarkEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

AOCRBMissile::AOCRBMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// 네트워크 리플리케이션 설정
	bReplicates = true;
	SetReplicateMovement(true);

    // 메시 컴포넌트
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(true);
	MeshComponent->SetHiddenInGame(false);
    RootComponent = MeshComponent;

    // 충돌 컴포넌트 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    CollisionComponent->SetSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetVisibility(true);
	CollisionComponent->SetupAttachment(MeshComponent);

    // 발사체 이동 컴포넌트
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->SetIsReplicated(true);
	    
    // 유도 설정
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;

    // 트레일 이펙트 
    TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
    TrailEffect->SetupAttachment(RootComponent);
    TrailEffect->bAutoActivate = true;

    // 수명 설정
    InitialLifeSpan = LifeSpan;

    // 충돌 이벤트 바인딩
    CollisionComponent->OnComponentHit.AddDynamic(this, &AOCRBMissile::OnHit);
}

void AOCRBMissile::BeginPlay()
{
	Super::BeginPlay();
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 12, FColor::Red, false, 10.0f, 0, 5.0f);
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
		NewTargetInTick=HomingTarget;
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
	// 자기 자신이나 발사자는 제외
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()) && OtherActor != GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());
		DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 12, FColor::Red, false, 10.0f, 0, 5.0f);
        
		// 이벤트 브로드캐스트
		FOnProHit.Broadcast(OtherActor, Hit);
        
		// 히트 이펙트 재생
		if (HitEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Hit.Location, Hit.Normal.Rotation());
		}
        
		// 히트 사운드 재생
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location);
		}

		// 발사체에서 직접 표식 적용
		ApplyMarkToTarget(OtherActor);
        
		// 발사체 제거
		Destroy();
	}
}

void AOCRBMissile::ApplyMarkToTarget(AActor* Target)
{
	if (!HasAuthority() || !MarkEffectClass || !Target) return;
    
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;
    
	// 발사자(Owner)의 ASC 사용
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!SourceASC) return;

	UE_LOG(LogTemp, Error, TEXT("🔍 BEFORE GE - All target tags: %s"), *TargetASC->GetOwnedGameplayTags().ToStringSimple())
	
	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(	MarkEffectClass, 1, EffectContext);
	
	//------이펙트 추가 적용
	//FGameplayEffectSpecHandle SpeedSpecHandle = MakeOutgoingGameplayEffectSpec(MarkEffectClass);
	// if (SpeedSpecHandle.IsValid() && SpeedSpecHandle.Data.IsValid())
	// {
	// 	FGameplayEffectSpec* Spec = SpeedSpecHandle.Data.Get();
	//
	// 	Spec->DynamicGrantedTags.AddTag(TAG_Item_SpeedBoost_Active);
	//
	// 	ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	//
	// 	bool bIsServer = (ASC->GetOwnerRole() == ROLE_Authority);
	//
	// 	FGameplayTagContainer OwnedTags;
	// 	ASC->GetOwnedGameplayTags(OwnedTags);
	// }



	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		Spec->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Marked")));
		//Target->ActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		//FGameplayTagContainer OwnedTags;
		//SourceASC->GetOwnedGameplayTags(OwnedTags);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		

		FActiveGameplayEffectHandle EffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		
		
		// 🔍 GE 스펙 정보 출력
		if (SpecHandle.Data.IsValid())
		{
			const UGameplayEffect* GE = SpecHandle.Data->Def;
			UE_LOG(LogTemp, Error, TEXT("🔍 GE Duration Policy: %d"), (int32)GE->DurationPolicy);
			UE_LOG(LogTemp, Error, TEXT("🔍 GE Granted Tags Count: %d"), GE->InheritableOwnedTagsContainer.CombinedTags.Num());
            
			for (const FGameplayTag& Tag : GE->InheritableOwnedTagsContainer.CombinedTags)
			{
				UE_LOG(LogTemp, Error, TEXT("🏷️ GE Granted Tag: %s"), *Tag.ToString());
			}
		}
		if (EffectHandle.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Mark applied to target: %s"), *Target->GetName());
			
			// 0.1초 후에 태그 확인 (타이밍 문제 해결)
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()
			{
				if (IsValid(Target) && IsValid(TargetASC))
				{
					bool bHasMarkTag = TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Marked")));
					UE_LOG(LogTemp, Warning, TEXT("Delayed tag check: %s"), bHasMarkTag ? TEXT("YES! 🎉") : TEXT("Still NO... 😢"));
				}
			}, 0.1f, false);
			
			//SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			// 효과 적용 직후 태그 확인
			//bool bHasMarkTag = TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Marked")));
		}
	}
}

void AOCRBMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// UE_LOG(LogTemp, Warning, TEXT("AOCRBMissile::Velocity: %s, Location: %s"), *ProjectileMovement->Velocity.ToString(),*GetActorLocation().ToString());
	// if (NewTargetInTick.IsValid())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("AOCRBMissile::NewTargetInTick: %s"), *NewTargetInTick->GetActorLocation().ToString());
	// }
}
