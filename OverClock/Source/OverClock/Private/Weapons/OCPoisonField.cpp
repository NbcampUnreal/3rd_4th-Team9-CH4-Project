// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OCPoisonField.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Components/SphereComponent.h"

AOCPoisonField::AOCPoisonField()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicatingMovement(true);
	
	Collision=CreateDefaultSubobject<USphereComponent>(FName("Collision"));
	Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent=Collision;
	
	FieldMesh=CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	FieldMesh->SetupAttachment(Collision);
	FieldMesh->SetWorldScale3D(FVector(0.f));
}

void AOCPoisonField::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(Duration);
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AOCPoisonField::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AOCPoisonField::OnOverlapEnd);

	GetWorldTimerManager().SetTimer(
		TickTimer,
		this,
		&AOCPoisonField::ApplyDamage,
		TickInterval,
		true
	);
}

void AOCPoisonField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TickTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(TickTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void AOCPoisonField::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == GetOwner() /* || IsTeam(OtherActor) */ ) return;
    
	if (APawn* TargetPawn = Cast<APawn>(OtherActor))
	{
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetPawn);
		if (!ASI) return;
		UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();

		if (TargetASC)
		{
			OverlappingASCs.AddUnique(TargetASC);
            
			if (StunEffect)
			{
				FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
                
				// 3초 지속 정지 GE 적용
				TargetASC->ApplyGameplayEffectToSelf(StunEffect.GetDefaultObject(), 1, Context);
			}
		}
	}
}

void AOCPoisonField::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
    
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
	if (!ASI) return;
	UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();

	if (TargetASC)
	{
		OverlappingASCs.Remove(TargetASC);
	}
}

bool AOCPoisonField::IsTeam(AActor* TargetActor) const
{
	if (!GetOwner()) return false;
	
	if (TargetActor->IsA(APawn::StaticClass()))
	{
		IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(TargetActor);
		if (!TargetASI) return false;
		IAbilitySystemInterface* OwnerASI = Cast<IAbilitySystemInterface>(GetOwner());
		if (!OwnerASI) return false;
	    
		UAbilitySystemComponent* OwnerASC = OwnerASI->GetAbilitySystemComponent();
		if (!OwnerASC) return false;
		UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
		if (!TargetASC) return false;

		FGameplayTagContainer TargetTags;
		TargetASC->GetOwnedGameplayTags(TargetTags);
		FGameplayTag TargetTag;
		for (const FGameplayTag& Tag : TargetTags)
		{
			if (Tag.ToString().StartsWith(TEXT("Team.")))
			{
				TargetTag=Tag;
			}
		}

		FGameplayTagContainer OwnerTags;
		TargetASC->GetOwnedGameplayTags(OwnerTags);
		FGameplayTag OwnerTag;
		for (const FGameplayTag& Tag : OwnerTags)
		{
			if (Tag.ToString().StartsWith(TEXT("Team.")))
			{
				OwnerTag=Tag;
			}
		}
		
		if(TargetTag == OwnerTag) return true;
	}
	return false;
}

void AOCPoisonField::ApplyDamage()
{
	if (!DamageEffect) return;
	for (UAbilitySystemComponent* ASC : OverlappingASCs)
	{
		if (ASC && ASC->GetOwnerActor() /* && !IsTeam(ASC->GetOwnerActor())*/ )
		{
			ASC->ApplyGameplayEffectToSelf(
				DamageEffect.GetDefaultObject(),
				1,
				ASC->MakeEffectContext());
		}
	}
}

void AOCPoisonField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaSum+= DeltaTime;

	float Alpha = FMath::Clamp(DeltaSum, 0.0f, 1.0f);
	float Scale = FMath::InterpEaseInOut(0.0f, 10.0f, Alpha, 3.0f);

	FieldMesh->SetWorldScale3D(FVector(Scale, Scale, Scale/2));

	if (Alpha >= 1.0f)
	{
		DeltaSum=0.0f;
		UE_LOG(LogTemp, Warning, TEXT("NetMode : %s"),*ToString(GetNetMode()))
		SetActorTickEnabled(false);
	}
}
