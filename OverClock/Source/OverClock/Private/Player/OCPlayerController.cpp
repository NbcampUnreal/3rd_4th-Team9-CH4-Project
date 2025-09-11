#include "Player/OCPlayerController.h"
#include "EnhancedInputSubsystems.h"

AOCPlayerController::AOCPlayerController() :
	PlayerInputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr),

	GANormalAttackAction(nullptr)
{ }

void AOCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (PlayerInputMappingContext)
			{
				Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
			}
		}
	}
}