#include "Core/Ch03_GameModeBase.h"
#include "Core/Ch03_CheonbokController.h"
#include "Core/Ch03_GameStateBase.h"
#include "Character/Ch03_CheonbokCharacter.h"

ACh03_GameModeBase::ACh03_GameModeBase()
{
	PlayerControllerClass = ACh03_CheonbokController::StaticClass();
	DefaultPawnClass = ACh03_CheonbokCharacter::StaticClass();
	GameStateClass = ACh03_GameStateBase::StaticClass();
}
