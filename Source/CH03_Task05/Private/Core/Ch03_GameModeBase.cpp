#include "Core/Ch03_GameModeBase.h"
#include "Character/Ch03_CheonbokCharacter.h"

ACh03_GameModeBase::ACh03_GameModeBase()
{
	DefaultPawnClass = ACh03_CheonbokCharacter::StaticClass();
}
