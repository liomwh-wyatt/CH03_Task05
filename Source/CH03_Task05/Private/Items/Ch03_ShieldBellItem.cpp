#include "Items/Ch03_ShieldBellItem.h"

#include "Character/Ch03_CheonbokCharacter.h"

ACh03_ShieldBellItem::ACh03_ShieldBellItem()
{
	ItemType = TEXT("ShieldBell");
	RotationSpeed = 120.0f;
	BobAmplitude = 20.0f;
	BobFrequency = 1.8f;
	LifetimeAfterSpawn = 10.0f;
}

void ACh03_ShieldBellItem::ActivateItem_Implementation(AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator))
	{
		return;
	}

	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator);
	if (!CheonbokCharacter
		|| !CheonbokCharacter->CanAddDamageShield(ShieldStackAmount))
	{
		return;
	}

	CheonbokCharacter->ApplyDamageShield(ShieldStackAmount);
	Super::ActivateItem_Implementation(Activator);
}
