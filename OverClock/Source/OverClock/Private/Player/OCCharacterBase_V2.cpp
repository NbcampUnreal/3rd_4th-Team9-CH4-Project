#include "Player/OCCharacterBase_V2.h"
#include "Player/OCPlayerState.h"
#include "Abilities/OCAbilitySystemComponent.h"
#include "Input/OCInputComponent.h"
#include "Data/DA_OCInputConfig.h"
#include "Data/DA_OCHeroStartUpData.h"
#include "Data/OCGameplayTags.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AOCCharacterBase_V2::AOCCharacterBase_V2()
{
    PrimaryActorTick.bCanEverTick = false;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetRootComponent());
    Camera->bUsePawnControlRotation = true;
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
}

UAbilitySystemComponent* AOCCharacterBase_V2::GetAbilitySystemComponent() const
{
    return GetOCASC();
}

void AOCCharacterBase_V2::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitASCFromPlayerState();

    if (HasAuthority())
    {
        ResolveData();
        if (UOCAbilitySystemComponent* ASC = GetOCASC())
        {
            if (HeroData)
            {
                HeroData->GiveToAbilitySystemComponent(ASC, 1);
            }
        }
    }
}

void AOCCharacterBase_V2::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitASCFromPlayerState();
}

const UDA_OCInputConfig* AOCCharacterBase_V2::GetInputConfig() const
{
    return InputConfig;
}

void AOCCharacterBase_V2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UOCInputComponent* OCIC = Cast<UOCInputComponent>(PlayerInputComponent);
    if (!ensureAlwaysMsgf(OCIC, TEXT("InputComponent must be UOCInputComponent"))) return;

    ResolveData();
    if (!ensureAlways(InputConfig)) return;
    
    OCIC->BindNativeInputAction(InputConfig, OCGameplayTags::InputTag_Move,  ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
    OCIC->BindNativeInputAction(InputConfig, OCGameplayTags::InputTag_Look,  ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
    OCIC->BindNativeInputAction(InputConfig, OCGameplayTags::InputTag_Jump,  ETriggerEvent::Started,   this, &ThisClass::Input_Jump_Pressed);
    OCIC->BindNativeInputAction(InputConfig, OCGameplayTags::InputTag_Jump,  ETriggerEvent::Completed, this, &ThisClass::Input_Jump_Released);
    
    OCIC->BindAbilityInputAction(InputConfig, this, &ThisClass::Input_Ability_Pressed, &ThisClass::Input_Ability_Released);
}

void AOCCharacterBase_V2::ResolveData()
{
    if (!InputConfig && InputConfigAsset.IsValid()) InputConfig = InputConfigAsset.Get();
    if (!InputConfig && InputConfigAsset.ToSoftObjectPath().IsValid()) InputConfig = InputConfigAsset.LoadSynchronous();

    if (!HeroData && HeroStartUpDataAsset.IsValid()) HeroData = HeroStartUpDataAsset.Get();
    if (!HeroData && HeroStartUpDataAsset.ToSoftObjectPath().IsValid()) HeroData = HeroStartUpDataAsset.LoadSynchronous();
}

void AOCCharacterBase_V2::InitASCFromPlayerState()
{
    AOCPlayerState* PS = GetPlayerState<AOCPlayerState>();
    if (!PS) return;
    
    UOCAbilitySystemComponent* ASC = Cast<UOCAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    if (!ASC) return;

    ASCWeak = ASC;
    ASC->InitAbilityActorInfo(PS, this);
}

UOCAbilitySystemComponent* AOCCharacterBase_V2::GetOCASC() const
{
    return ASCWeak.Get();
}

/* ===== Native Inputs ===== */

void AOCCharacterBase_V2::Input_Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Axis.IsNearlyZero()) return;

    const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
    if (!FMath::IsNearlyZero(Axis.X))
    {
        const FVector Forward = YawRot.RotateVector(FVector::ForwardVector);
        AddMovementInput(Forward, Axis.X);
    }
    if (!FMath::IsNearlyZero(Axis.Y))
    {
        const FVector Right = YawRot.RotateVector(FVector::RightVector);
        AddMovementInput(Right, Axis.Y);
    }
}

void AOCCharacterBase_V2::Input_Look(const FInputActionValue& Value)
{
    const FVector2D Look = Value.Get<FVector2D>();
    if (!FMath::IsNearlyZero(Look.X)) AddControllerYawInput(Look.X);
    if (!FMath::IsNearlyZero(Look.Y)) AddControllerPitchInput(Look.Y);
}

void AOCCharacterBase_V2::Input_Jump_Pressed(const FInputActionValue&)
{
    Jump();
}

void AOCCharacterBase_V2::Input_Jump_Released(const FInputActionValue&)
{
    StopJumping();
}

/* ===== Ability Inputs (Tag) ===== */

void AOCCharacterBase_V2::Input_Ability_Pressed(FGameplayTag InInputTag)
{
    if (!InInputTag.IsValid()) return;

    if (UOCAbilitySystemComponent* ASC = GetOCASC())
    {
        ASC->OnAbilityInputPressed(InInputTag);
    }

    Server_Ability_Pressed(InInputTag);
}

void AOCCharacterBase_V2::Input_Ability_Released(FGameplayTag InInputTag)
{
    if (!InInputTag.IsValid()) return;

    if (UOCAbilitySystemComponent* ASC = GetOCASC())
    {
        ASC->OnAbilityInputReleased(InInputTag);
    }
    Server_Ability_Released(InInputTag);
}

/* ===== Server RPC ===== */

void AOCCharacterBase_V2::Server_Ability_Pressed_Implementation(FGameplayTag InInputTag)
{
    if (!InInputTag.IsValid()) return;

    if (UOCAbilitySystemComponent* ASC = GetOCASC())
    {
        ASC->OnAbilityInputPressed(InInputTag);
    }
}

void AOCCharacterBase_V2::Server_Ability_Released_Implementation(FGameplayTag InInputTag)
{
    if (!InInputTag.IsValid()) return;

    if (UOCAbilitySystemComponent* ASC = GetOCASC())
    {
        ASC->OnAbilityInputReleased(InInputTag);
    }
}
