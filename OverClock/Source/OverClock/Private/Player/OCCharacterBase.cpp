#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "OverClockDebugHelper.h"
#include "Net/UnrealNetwork.h"


AOCCharacterBase::AOCCharacterBase()
	:WalkSpeed(600.0f),
	RunSpeed(900.0f),
	JumpVelocity(600.0f)
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