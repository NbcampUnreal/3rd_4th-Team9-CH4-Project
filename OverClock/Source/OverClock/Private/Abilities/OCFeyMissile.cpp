#include "Abilities/OCFeyMissile.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/GameplayStatics.h"

AOCFeyMissile::AOCFeyMissile()
{
	PrimaryActorTick.bCanEverTick = true;

    InitialSpeed = 2000.0f;
    MaxSpeed = 2500.0f;
    LifeSpan = 5.0f;

    // 충돌 설정
    CollisionComponent->SetSphereRadius(20.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AOCFeyMissile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOCFeyMissile::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    CollisionComponent->OnComponentHit.AddDynamic(this, &AOCFeyMissile::OnHit);
}

void AOCFeyMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

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

        // Damage 적용
        FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
        EffectContext.AddSourceObject(this);
        
        // TODO: DamageEffect 적용
        // FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, EffectContext);
        // if (SpecHandle.IsValid())
        // {
        //     SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), Damage);
        //     TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        // }
	    
        Destroy();
        
    }    
}

void AOCFeyMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}