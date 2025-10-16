#include "Player/OCTheFey.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Actors/OCWhipMissile.h"

AOCTheFey::AOCTheFey()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	WeaponRoot->SetupAttachment(GetMesh(), TEXT("Muzzle_01"));
    
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetWorldScale3D(FVector(0.0f));

	bIsScaling=true;
	DeltaSum=0.f;
	WhipPoolSize=10;
	GemPoolSize=10;
}

void AOCTheFey::BeginPlay()
{
	Super::BeginPlay();
	WhipPool = MakeUnique<TItemPool<AOCWhipMissile>>(GetWorld(), WhipPoolSize);
	WhipPool->AllocateAll(WhipPoolClass, WhipPoolSize);
	GemPool = MakeUnique<TItemPool<AOCGemMissile>>(GetWorld(), GemPoolSize);
	GemPool->AllocateAll(GemPoolClass, GemPoolSize);
}

void AOCTheFey::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (HasAuthority()) GiveStartupAbilities();
}

void AOCTheFey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsScaling)
	{
		DeltaSum+= DeltaTime;

		float Alpha = FMath::Clamp(DeltaSum, 0.0f, 1.0f);
		float Scale = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);

		WeaponMesh->SetWorldScale3D(FVector(Scale));

		if (Alpha >= 1.0f)
		{
			DeltaSum=0.0f;
			bIsScaling = false;
			SetActorTickEnabled(false);
		}
	}
}

void AOCTheFey::GiveStartAbilities()
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(this))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (HealWhipAbilityClass && !ASC->FindAbilitySpecFromClass(HealWhipAbilityClass))
			{
				ASC->GiveAbility(FGameplayAbilitySpec(HealWhipAbilityClass, 1, 0, this));
			}
			if (HealGemAbilityClass && !ASC->FindAbilitySpecFromClass(HealGemAbilityClass))
			{
				ASC->GiveAbility(FGameplayAbilitySpec(HealGemAbilityClass, 1, 0, this));
			}
			if (PoisonSeedAbilityClass && !ASC->FindAbilitySpecFromClass(PoisonSeedAbilityClass))
			{
				ASC->GiveAbility(FGameplayAbilitySpec(PoisonSeedAbilityClass, 1, 0, this));
			}
			if (WildStormAbilityClass && !ASC->FindAbilitySpecFromClass(WildStormAbilityClass))
			{
				ASC->GiveAbility(FGameplayAbilitySpec(WildStormAbilityClass, 1, 0, this));
			}
		}
	}
}

AOCWhipMissile* AOCTheFey::GetWhipMissileFromPool()
{
	if (!HasAuthority()) return nullptr;
	if (!WhipPool) return nullptr;
	AOCWhipMissile* Missile = WhipPool->GetPooledActor();
	if (!IsValid(Missile)) return nullptr;

	const FVector MuzzleLocation = WeaponRoot->GetComponentLocation();
	Missile->SetActorLocationAndRotation(MuzzleLocation, FRotator::ZeroRotator);
	
	Missile->Init();
	
	return Missile;
}

AOCGemMissile* AOCTheFey::GetGemMissileFromPool()
{
	if (!WhipPool) return nullptr;
	if (!GemPool) return nullptr;
	AOCGemMissile* Missile = GemPool->GetPooledActor();
	const FVector MuzzleLocation = WeaponRoot->GetComponentLocation();
	const FRotator MuzzleRotation = WeaponRoot->GetComponentRotation();

	if (!IsValid(Missile)) return nullptr;
	Missile->SetActorLocationAndRotation(MuzzleLocation, MuzzleRotation);
	
	Missile->Init();
	
	return Missile;
}

void AOCTheFey::ScalingWeapon()
{
	bIsScaling=true;
	SetActorTickEnabled(true);
}

void AOCTheFey::HiddenWeapon()
{
	WeaponMesh->SetWorldScale3D(FVector(0.0f));
}