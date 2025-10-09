#include "Player/OCPlayerController.h"
#include "EnhancedInputSubsystems.h"

void AOCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	AddDefaultMapping();
}

void AOCPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AddDefaultMapping();
}

void AOCPlayerController::OnUnPossess()
{
	RemoveDefaultMapping();
	Super::OnUnPossess();
}

void AOCPlayerController::AddDefaultMapping() const
{
	if (!IsLocalController() || !DefaultMappingContext) return;

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AOCPlayerController::RemoveDefaultMapping() const
{
	if (!IsLocalController() || !DefaultMappingContext) return;

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
		}
	}
}
