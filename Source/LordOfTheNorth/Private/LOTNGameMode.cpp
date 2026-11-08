#include "LOTNGameMode.h"
#include "LOTNCharacter.h"
#include "LOTNPlayerController.h"

ALOTNGameMode::ALOTNGameMode()
{
    DefaultPawnClass = ALOTNCharacter::StaticClass();
    PlayerControllerClass = ALOTNPlayerController::StaticClass();
}
