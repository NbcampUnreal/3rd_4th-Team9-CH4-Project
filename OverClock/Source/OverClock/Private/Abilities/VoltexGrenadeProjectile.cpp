#include "Abilities/VoltexGrenadeProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AVoltexGrenadeProjectile::AVoltexGrenadeProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(10.f);
    Collision->SetCollisionProfileName(TEXT("Projectile"));
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetNotifyRigidBodyCollision(true);

    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    RootComponent = Collision;

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = 1200.f;
    Movement->MaxSpeed = 1200.f;
    Movement->bRotationFollowsVelocity = true;
    Movement->ProjectileGravityScale = 0.0f;

    Collision->OnComponentHit.AddDynamic(this, &AVoltexGrenadeProjectile::OnHit);
}

void AVoltexGrenadeProjectile::BeginPlay()
{
    Super::BeginPlay();
    if (AActor* Ow = GetOwner())
    {
        Collision->IgnoreActorWhenMoving(Ow, true);
    }
    if(APawn* Inst = GetInstigator())
    {
        if (Inst != GetOwner())
        {
            Collision->IgnoreActorWhenMoving(Inst, true);
        }
    }
}

void AVoltexGrenadeProjectile::InitVelocity(const FVector& Dir, float Speed)
{
    if (Movement)
    {
        Movement->Velocity = Dir.GetSafeNormal() * Speed;
        Movement->InitialSpeed = Speed;
        Movement->MaxSpeed = Speed;
    }
}

void AVoltexGrenadeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == nullptr || OtherActor == GetOwner() || OtherActor == GetInstigator())
        return;
   
    if (HasAuthority())
    {      
        ExplodeAt(Hit.ImpactPoint);
    }
}

void AVoltexGrenadeProjectile::ExplodeAt(const FVector& ImpactLoc)
{
    if (bExploded) return;

    bExploded = true;

    if (!HasAuthority())
    {
        return;
    }

    AActor* InstigatorActor = GetOwner();
    UAbilitySystemComponent * InstASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);

    TArray<AActor*> HitActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes;

    ObjTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    ObjTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);

    if (InstigatorActor) { IgnoreActors.Add(InstigatorActor); }
    
    UKismetSystemLibrary::SphereOverlapActors(GetWorld(), ImpactLoc, ExplosionRadius, ObjTypes, AActor::StaticClass(), IgnoreActors, HitActors);

    FGameplayEffectSpecHandle Spec = InstASC->MakeOutgoingSpec(DamageEffect, /*Level*/1.f, InstASC->MakeEffectContext());
    if (Spec.IsValid())
    {
        for (AActor* A : HitActors)
        {
            if (!A || A == InstigatorActor) continue;

            if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(A))
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
            }
        }
    }

    Multicast_ExplosionFX(ImpactLoc);

    Destroy();
}

void AVoltexGrenadeProjectile::Multicast_ExplosionFX_Implementation(const FVector_NetQuantize Location)
{
    if (ExplosionNiagara)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionNiagara, Location, FRotator::ZeroRotator, FVector(ExplosionScale));
    }
    if (ExplosionSFX)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSFX, Location);
    }
}