#include "Actors/OCBarrierField.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Net/UnrealNetwork.h"

#include "Abilities/OCAbilitySystemComponent.h"
#include "Abilities/Attributes/OCAttributeSet_Health.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"

#include"GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

#include "Interface/OCBarrierInteract.h"

AOCBarrierField::AOCBarrierField()
{
	bReplicates = true;

	BarrierVolume = CreateDefaultSubobject<USphereComponent>(TEXT("BarrierVolume"));
	SetRootComponent(BarrierVolume);

	BarrierVolume->InitSphereRadius(Radius);
	BarrierVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BarrierVolume->SetCollisionObjectType(ECC_WorldDynamic);
	
	BarrierVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	BarrierVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BarrierVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	
	BarrierVolume->OnComponentBeginOverlap.AddDynamic(this, &AOCBarrierField::OnBarrierBeginOverlap);
	
	BarrierPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BarrierPSC"));
	BarrierPSC->SetupAttachment(BarrierVolume);
	BarrierPSC->bAutoActivate = false;
	BarrierPSC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BarrierPSC->SetGenerateOverlapEvents(false);
	
	ASC = CreateDefaultSubobject<UOCAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = CreateDefaultSubobject<UOCAttributeSet_Health>(TEXT("HealthSet"));
}

void AOCBarrierField::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOCBarrierField, OwnerController);
}

void AOCBarrierField::InitOwnerController(AController* InOwnerController)
{
	if (HasAuthority())
	{
		OwnerController = InOwnerController;
	}
}

void AOCBarrierField::InitParams(float InRadius, float InHalfHeight, float InLifeSeconds, float InMaxHealth)
{
	Radius      = InRadius;
	LifeSeconds = InLifeSeconds;
	MaxHealth   = InMaxHealth;

	if (BarrierVolume)
	{
		BarrierVolume->SetSphereRadius(Radius);
	}
}

void AOCBarrierField::BeginPlay()
{
	Super::BeginPlay();
	
	BarrierVolume->SetCollisionResponseToChannel(ECC_Visibility, BarrierPolicy.bBlockHitScan ? ECR_Block : ECR_Ignore);
	
	if (BarrierPSC)
	{
		if (BarrierEffect) { BarrierPSC->SetTemplate(BarrierEffect); }
		if (EffectUniformScale > 0.f) { BarrierPSC->SetWorldScale3D(FVector(EffectUniformScale)); }
		BarrierPSC->Activate(true);

		if (FormationDelay > 0.f)
		{
			GetWorldTimerManager().SetTimer(TH_Pause, this, &AOCBarrierField::PauseEffect, FormationDelay, false);
		}
	}
	
	if (HasAuthority())
	{
		ASC->InitAbilityActorInfo(this, this);

		if (InitHealthGE)
		{
			const FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			ASC->ApplyGameplayEffectToSelf(InitHealthGE->GetDefaultObject<UGameplayEffect>(), 1.f, Ctx);
		}
		else
		{
			ASC->SetNumericAttributeBase(UOCAttributeSet_Health::GetMaxHealthAttribute(), MaxHealth);
			ASC->SetNumericAttributeBase(UOCAttributeSet_Health::GetHealthAttribute(),   MaxHealth);
		}

		ASC->GetGameplayAttributeValueChangeDelegate(UOCAttributeSet_Health::GetHealthAttribute())
			.AddUObject(this, &AOCBarrierField::OnHealthChanged);

		if (LifeSeconds > 0.f)
		{
			GetWorldTimerManager().SetTimer(TH_Life, this, &AOCBarrierField::OnLifeExpired, LifeSeconds, false);
		}
	}
}

void AOCBarrierField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(TH_Life);
	GetWorldTimerManager().ClearTimer(TH_Pause);
	Super::EndPlay(EndPlayReason);
}

void AOCBarrierField::PauseEffect()
{
	if (BarrierPSC)
	{
		BarrierPSC->SetComponentTickEnabled(false);
	}
}

bool AOCBarrierField::IsFriendlyActor(const AActor* Other) const
{
	const APawn* Pawn = Other ? Cast<APawn>(Other->GetInstigator()) : nullptr;
	return (Pawn && OwnerController && Pawn->GetController() == OwnerController);
}

void AOCBarrierField::ApplyBarrierDamage(float Damage, AActor* DamageSource, const FHitResult& Where)
{
	if (!HasAuthority() || Damage <= 0.f) return;

	if (DamageTakenGE && SetByCaller_DamageTag.IsValid())
	{
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(DamageSource);

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(DamageTakenGE, 1.f, Ctx);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(SetByCaller_DamageTag, Damage);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			return;
		}
	}
	
	const float Current = HealthSet ? HealthSet->GetHealth() : 0.f;
	const float NewVal  = FMath::Max(0.f, Current - Damage);
	ASC->SetNumericAttributeBase(UOCAttributeSet_Health::GetHealthAttribute(), NewVal);
}

void AOCBarrierField::ApplyHitscanImpact(float Damage, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (BarrierPolicy.bAllowFriendly && IsFriendlyActor(InstigatorActor))
	{
		return;
	}
	ApplyBarrierDamage(Damage, InstigatorActor, Hit);
}

void AOCBarrierField::ApplyAbilityImpact(float Damage, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (BarrierPolicy.bAllowFriendly && IsFriendlyActor(InstigatorActor))
	{
		return;
	}
	ApplyBarrierDamage(Damage, InstigatorActor, Hit);
}

AController* AOCBarrierField::GetOwnerControllerCached() const
{
	return OwnerController;
}

void AOCBarrierField::OnLifeExpired()
{
	Destroy();
}

void AOCBarrierField::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (HealthSet && HealthSet->GetHealth() <= 0.f)
	{
		Destroy();
	}
}

void AOCBarrierField::OnBarrierBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	if (!HasAuthority() || OtherActor == nullptr) return;
	
	const bool bProjectile = (OtherActor->GetComponentByClass(UProjectileMovementComponent::StaticClass()) != nullptr);
	if (!bProjectile) return;
	
	if (BarrierPolicy.bAllowFriendly && IsFriendlyActor(OtherActor))
	{
		return;
	}
	
	float DamageToBarrier = 0.f;
	EOCBarrierHitResponse Response = EOCBarrierHitResponse::DestroySelf;

	if (OtherActor->GetClass()->ImplementsInterface(UOCBarrierInteract::StaticClass()))
	{
		DamageToBarrier = IOCBarrierInteract::Execute_GetBarrierDamage(OtherActor);
		Response        = IOCBarrierInteract::Execute_GetBarrierHitResponse(OtherActor);
	}
	
	if (DamageToBarrier > 0.f)
	{
		ApplyBarrierDamage(DamageToBarrier, OtherActor, SweepResult);
	}
	
	switch (Response)
	{
	case EOCBarrierHitResponse::ExplodeAtContact:
		if (OtherActor->GetClass()->ImplementsInterface(UOCBarrierInteract::StaticClass()))
		{
			IOCBarrierInteract::Execute_OnBarrierContact(OtherActor, this, SweepResult);
		}
		else
		{
			OtherActor->Destroy();
		}
		break;

	case EOCBarrierHitResponse::DestroySelf:
	default:
		OtherActor->Destroy();
		break;

	case EOCBarrierHitResponse::Ignore:
		break;
	}
}