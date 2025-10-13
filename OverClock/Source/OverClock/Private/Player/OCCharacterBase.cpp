#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "OverClockDebugHelper.h"
#include "Abilities/OCMarkComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemInterface.h"


AOCCharacterBase::AOCCharacterBase()
	:WalkSpeed(600.0f),
	RunSpeed(900.0f),
	JumpVelocity(600.0f),
	AimPitch(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true;

	MarkComp = CreateDefaultSubobject<UOCMarkComponent>(TEXT("StatusMark"));
	MarkComp->SetupAttachment(RootComponent);
	
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
	if (!HasAuthority())
	{
		UpdateRotation();
	}
}

void AOCCharacterBase::OnStunTagChanged(const FGameplayTag GameplayTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->DisableMovement();
			MoveComp->StopMovementImmediately();
		}
	}
	else
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
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
		
		const FString ASCText = FString::Printf(TEXT("Owner Actor : %s, AvatarActor : %s"), *GetAbilitySystemComponent()->GetOwnerActor()->GetActorLabel(), *GetAbilitySystemComponent()->GetAvatarActor()->GetActorLabel());
		Debug::Print(TEXT("Ability system component valid") + ASCText, FColor::Green);
	}
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// ASC의 태그 변화 바인딩
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Stun")), EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AOCCharacterBase::OnStunTagChanged);
	}
	MarkComp->ASCBind();
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
	
	DOREPLIFETIME(AOCCharacterBase, AimPitch);
}

float AOCCharacterBase::GetAimPitch() const//AnimInstance Tick
{
	if (GetNetMode()==ROLE_AutonomousProxy)
	{
		return GetControlRotation().Pitch;
	}
	return AimPitch;
}

void AOCCharacterBase::UpdateRotation()//Tick
{
	if (FMath::Abs(RotationTemp - GetControlRotation().Pitch) > 1.f)
	{
		RotationTemp=GetControlRotation().Pitch;
		ServerSetAimPitch(RotationTemp);
	}
}

void AOCCharacterBase::ServerSetAimPitch_Implementation(float InAimPitch)
{
	AimPitch=InAimPitch;
}
