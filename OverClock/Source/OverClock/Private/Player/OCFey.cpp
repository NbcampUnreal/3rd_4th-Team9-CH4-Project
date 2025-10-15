#include "Player/OCFey.h"

#include "AbilitySystemComponent.h"

AOCFey::AOCFey()
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

AOCFey::~AOCFey(){}

void AOCFey::BeginPlay()
{
	Super::BeginPlay();
	WhipPool = MakeUnique<TItemPool<AOCWhipMissile>>(GetWorld(), WhipPoolSize);
	WhipPool->AllocateAll(WhipPoolClass, WhipPoolSize);
	GemPool = MakeUnique<TItemPool<AOCGemMissile>>(GetWorld(), GemPoolSize);
	GemPool->AllocateAll(GemPoolClass, GemPoolSize);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	// 추가/제거 모두 감지
	ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Hidden")), EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AOCFey::OnHiddenTagChanged);
	ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Scaling")), EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AOCFey::OnScalingTagChanged);
}

void AOCFey::Tick(float DeltaTime)
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

void AOCFey::OnHiddenTagChanged(const FGameplayTag GameplayTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		WeaponMesh->SetWorldScale3D(FVector(0.0f));
	}
	else
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (!ASC) return;
		
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		ASC->ApplyGameplayEffectToSelf(ScalingEffect.GetDefaultObject(),1,EffectContext);
	}
}

void AOCFey::OnScalingTagChanged(const FGameplayTag GameplayTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		bIsScaling=true;
		SetActorTickEnabled(true);
	}
}

AOCWhipMissile* AOCFey::GetWhipMissileFromPool()
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

AOCGemMissile* AOCFey::GetGemMissileFromPool()
{
	if (!HasAuthority()) return nullptr;
	if (!GemPool) return nullptr;
	AOCGemMissile* Missile = GemPool->GetPooledActor();
	const FVector MuzzleLocation = WeaponRoot->GetComponentLocation();
	const FRotator MuzzleRotation = WeaponRoot->GetComponentRotation();

	if (!IsValid(Missile)) return nullptr;
	Missile->SetActorLocationAndRotation(MuzzleLocation, MuzzleRotation);

	Missile->Init();
	
	return Missile;
}