#include "Game/GameModes/OCGameModeBase_Min.h"

#include "Player/OCCharacterBase.h"
#include "Player/OCPlayerController.h"
#include "Player/OCPlayerState.h"

AOCGameModeBase_Min::AOCGameModeBase_Min()
{
	DefaultPawnClass = AOCCharacterBase::StaticClass();
	PlayerControllerClass = AOCPlayerController::StaticClass();
	PlayerStateClass = AOCPlayerState::StaticClass();
}
