#include "Items/Ch03_TangledYarnItem.h"

#include "Character/Ch03_CheonbokCharacter.h"

ACh03_TangledYarnItem::ACh03_TangledYarnItem()
{
	ItemType = TEXT("TangledYarn");
	RotationSpeed = 80.0f;
	BobAmplitude = 16.0f;
	BobFrequency = 1.4f;
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

	Super::ActivateItem_Implementation(Activator);
}
