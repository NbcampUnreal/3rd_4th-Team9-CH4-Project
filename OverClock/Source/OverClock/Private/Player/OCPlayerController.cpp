#include "Player/OCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Player/OCCharacterBase_V2.h"
#include "Data/DA_OCInputConfig.h"
#include "Component/OCUIComponent.h"

// #include "InputActionValue.h"
// #include "GameplayTagContainer.h"
// #include "GameFramework/Pawn.h"
// #include "GameFramework/Character.h"
// #include "Player/OCCharacterBase.h"
// #include "Player/OCPlayerState.h"
// #include "GA/GA_RangedAttack.h"
// #include "Abilities/OCAbilityDataAsset.h"
// #include "GameFramework/InputSettings.h"
// #include "Player/OCRevenant.h"
// #include "OverClock.h"

namespace
{
	static UOCUIComponent* FindHUDComp(APawn* Pawn)
	{
		return Pawn->FindComponentByClass<UOCUIComponent>();
	}
}

AOCPlayerController::AOCPlayerController()
{
    bReplicates = true;
}

void AOCPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

	if (IsLocalController() && InPawn)
	{
		ApplyIMCForPawn(InPawn);
		SetInputMode(FInputModeGameOnly{});
		bShowMouseCursor = false;

		if (ActiveUIComp.IsValid())
		{
			ActiveUIComp->HideUI();
			ActiveUIComp = nullptr;
		}
		if (UOCUIComponent* NewComp = FindHUDComp(InPawn))
		{
			NewComp->InitializeForOwner();
			NewComp->ShowUI();
			ActiveUIComp = NewComp;
		}
	}
}

void AOCPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (IsLocalController() && GetPawn())
	{
		ApplyIMCForPawn(GetPawn());
		SetInputMode(FInputModeGameOnly{});
		bShowMouseCursor = false;

		if (ActiveUIComp.IsValid())
		{
			ActiveUIComp->HideUI();
			ActiveUIComp = nullptr;
		}
		if (UOCUIComponent* NewComp = FindHUDComp(GetPawn()))
		{
			NewComp->InitializeForOwner();
			NewComp->ShowUI();
			ActiveUIComp = NewComp;
		}
	}
}

void AOCPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

	if (IsLocalController())
	{
		RemoveMappingContext();

		if (ActiveUIComp.IsValid())
		{
			ActiveUIComp->HideUI();
			ActiveUIComp = nullptr;
		}
	}
}


UInputMappingContext* AOCPlayerController::ResolvePawnMappingContext(APawn* InPawn) const
{
    if (!InPawn) return nullptr;
    
    if (const AOCCharacterBase_V2* V2 = Cast<AOCCharacterBase_V2>(InPawn))
    {
        if (const UDA_OCInputConfig* IC = V2->GetInputConfig())
        {
            return IC->DefaultMappingContext;
        }
    }
    
    return FallbackMappingContext;
}

void AOCPlayerController::ApplyIMCForPawn(APawn* InPawn)
{
	if (!IsLocalController() || !InPawn) return;

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* Subsys = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!Subsys) return;

	RemoveMappingContext();

	UInputMappingContext* IMC = ResolvePawnMappingContext(InPawn);

	if (!IMC) return;

	Subsys->AddMappingContext(IMC, MappingPriority);
	AppliedMappingContext = IMC;
}

void AOCPlayerController::RemoveMappingContext()
{
    if (!AppliedMappingContext) return;

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
	    if (UEnhancedInputLocalPlayerSubsystem* Subsys = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	    {
		    Subsys->RemoveMappingContext(AppliedMappingContext);
	    }
    }
	
    AppliedMappingContext = nullptr;
}
