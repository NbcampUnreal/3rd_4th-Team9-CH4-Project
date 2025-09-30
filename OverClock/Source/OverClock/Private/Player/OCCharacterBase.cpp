#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "OverClockDebugHelper.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Data/DA_OCHeroStartUpData.h"
#include "Net/UnrealNetwork.h"


AOCCharacterBase::AOCCharacterBase()
	:WalkSpeed(600.0f),
	RunSpeed(900.0f),
	JumpVelocity(600.0f),
	AimRotation(FRotator::ZeroRotator)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true;
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void AOCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AOCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		ServerSetAimRotation_Implementation(GetControlRotation());
	}
}

void AOCCharacterBase::GiveStartupIfServer()
{
	if (!HasAuthority()) return;

	AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
	if (!PS) return;

	if (UOCAbilitySystemComponent* ASC = Cast<UOCAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
	{
		if (!HeroStartUpData.IsNull())
		{
			if (UDA_OCHeroStartUpData* Data = HeroStartUpData.LoadSynchronous())
			{
				Data->GiveToAbilitySystemComponent(ASC, 1);
			}
		}
	}
}

UAbilitySystemComponent* AOCCharacterBase::GetAbilitySystemComponent() const
{
	if (const AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}


void AOCCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
		GiveStartupIfServer();
		
		const FString ASCText = FString::Printf(TEXT("Owner Actor : %s, AvatarActor : %s"), *GetAbilitySystemComponent()->GetOwnerActor()->GetActorLabel(), *GetAbilitySystemComponent()->GetAvatarActor()->GetActorLabel());
		Debug::Print(TEXT("Ability system component valid") + ASCText, FColor::Green);
	}
}

void AOCCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AOCPlayerState* PS = GetPlayerState<AOCPlayerState>())
	{
		PS->InitASCForAvatar(this);
	}
}

void AOCCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AOCCharacterBase, AimRotation);
}

void AOCCharacterBase::ServerSetAimRotation_Implementation(FRotator InAimRotation)
{
	AimRotation=InAimRotation;
}
