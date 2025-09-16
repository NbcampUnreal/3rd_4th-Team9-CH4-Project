#include "Player/OCCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/OCGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Input/OCInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// OCAttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AOCCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOCCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		for (const auto& AbilityClass: StartupAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
		}
	}
}

void AOCCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	ensure(InputConfigDataAsset || PlayerInputComponent);

	if (ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
			UOCInputComponent* OCInputComponent = CastChecked<UOCInputComponent>(PlayerInputComponent);
			
			OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Move);
			OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Look);
			OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::StartJump);
			OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Jump, ETriggerEvent::Completed, this, &ThisClass::StopJump);
			OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Sprint, ETriggerEvent::Triggered, this, &ThisClass::StartSprint);
			OCInputComponent->BindNativeInputAction(InputConfigDataAsset, OCGameplayTags::InputTag_Sprint, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
		}
	}
}

void AOCCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOCCharacterBase, AimRotation)
}

void AOCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ServerSetAimRotation(GetControlRotation());
}

void AOCCharacterBase::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AOCCharacterBase::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AOCCharacterBase::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AOCCharacterBase::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AOCCharacterBase::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AOCCharacterBase::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

FRotator AOCCharacterBase::GetAimRotation()
{
	if (AimRotation.Pitch > 180.f)
	{
		AimRotation.Pitch -= 360.f;
	}

	ensure(AimRotation.Pitch<-361.f);

	return AimRotation;
}

void AOCCharacterBase::ServerSetAimRotation_Implementation(FRotator InAimRotation)
{	
	AimRotation=InAimRotation;
}
