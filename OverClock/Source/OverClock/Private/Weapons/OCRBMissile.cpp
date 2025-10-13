#include "Weapons/OCRBMissile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "GameplayEffect.h"
#include "Data/OCGameplayTags.h"

AOCRBMissile::AOCRBMissile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 트레일 이펙트
	TrailEffect = CreateDefaultSubobject<UNiagaraSystem>(TEXT("TrailEffect"));
}

void AOCRBMissile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	// 충돌 이벤트 바인딩
	CollisionComponent->OnComponentHit.AddDynamic(this, &AOCRBMissile::OnHit);

	// Missile 이동 컴포넌트
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;

}

void AOCRBMissile::SetTarget(AActor* NewTarget)//외부에서 호출
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
	if (!HasAuthority() || !GameEffectClass || !OtherActor) return;

	// 자기 자신이나 발사자는 제외
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()) && OtherActor != GetOwner())
	{
		//같은 태그인지 확인
		IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(OtherActor);
		if (!TargetASI) return;
		IAbilitySystemInterface* OwnerASI = Cast<IAbilitySystemInterface>(GetOwner());
		if (!OwnerASI) return;
    
		UAbilitySystemComponent* OwnerASC = OwnerASI->GetAbilitySystemComponent();
		if (!OwnerASC) return;
		UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
		if (!TargetASC) return;
		
		bool Team1 = OwnerASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Red")));
		bool Team2 = TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Team.Red")));
		if(Team1 && Team2) return;
	
		// 히트 사운드 재생
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location);
		}

		// 표식 적용
		ApplyMarkToTarget(OwnerASC, TargetASC);
        
		Destroy();
	}
}

void AOCRBMissile::ApplyMarkToTarget(UAbilitySystemComponent* InOwnerASC, UAbilitySystemComponent* InTargetASC)
{
	FGameplayEffectContextHandle EffectContext = InOwnerASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = InOwnerASC->MakeOutgoingSpec(GameEffectClass, 1, EffectContext);
	
	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		Spec->DynamicGrantedTags.AddTag(OCGameplayTags::State_Marked);
		InOwnerASC->ApplyGameplayEffectSpecToTarget(*Spec, InTargetASC);
	}
}