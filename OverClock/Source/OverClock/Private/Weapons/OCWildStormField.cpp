#include "Weapons/OCWildStormField.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"

AOCWildStormField::AOCWildStormField()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->bAutoActivate = true;
}

void AOCWildStormField::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeTime);

	GetWorldTimerManager().SetTimer(
		GETimer, 
		this, 
		&AOCWildStormField::ApplyGE,
		1.0f,
		true
	);
}

FGameplayTag AOCWildStormField::GetTeamTag(UAbilitySystemComponent* ASC)
{
	FGameplayTagContainer TargetTags;
	ASC->GetOwnedGameplayTags(TargetTags);
	for (const FGameplayTag& Tag : TargetTags)
	{
		if (Tag.ToString().StartsWith(TEXT("Team.")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void AOCWildStormField::ApplyGE()
{
	if (!DamageGE || !HealGE || !SphereCollision) return;

	TArray<AActor*> OverlappingActors;
	SphereCollision->GetOverlappingActors(OverlappingActors);
	
	FGameplayTag OwnerTeamTag;
	if (IAbilitySystemInterface* OwnerASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		if (UAbilitySystemComponent* OwnerASC = OwnerASI->GetAbilitySystemComponent())
		{
			OwnerTeamTag = GetTeamTag(OwnerASC);
		}
	}

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor) continue;
		
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
				EffectContext.AddSourceObject(this);
				if (GetTeamTag(ASC)==OwnerTeamTag)
				{
					// Damage GE 적용                    
                    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
                    	DamageGE, 
                    	1.0f,  // Level
                    	EffectContext
                    );
                    if (SpecHandle.IsValid())
                    {
                    	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                    }
				}
				else
				{
					//Heal GE 적용                    
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
						HealGE, 
						1.0f,  // Level
						EffectContext
					);
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}
}

void AOCWildStormField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

