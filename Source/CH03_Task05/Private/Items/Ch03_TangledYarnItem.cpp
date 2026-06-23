#include "Items/Ch03_TangledYarnItem.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Core/Ch03_GameStateBase.h"

ACh03_TangledYarnItem::ACh03_TangledYarnItem()
{
	ItemType = TEXT("TangledYarn");
	RotationSpeed = 80.0f;
	BobAmplitude = 16.0f;
	BobFrequency = 1.4f;
	LifetimeAfterSpawn = 8.0f;
	BlinkStartTime = 2.0f;
}

void ACh03_TangledYarnItem::ActivateItem_Implementation(AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator))
	{
		return;
	}

	if (ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator))
	{
		CheonbokCharacter->ApplyMovementLock(MovementLockDuration);
	}

	if (ACh03_GameStateBase* CheonbokGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr)
	{
		CheonbokGameState->BreakCombo();
	}

	Super::ActivateItem_Implementation(Activator);
}
