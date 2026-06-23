#include "Items/Ch03_HealingItem.h"

#include "Character/Ch03_CheonbokCharacter.h"

ACh03_HealingItem::ACh03_HealingItem()
{
	ItemType = TEXT("HealingItem");
	LifetimeAfterSpawn = 14.0f;
}

void ACh03_HealingItem::ActivateItem_Implementation(AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator) || HealingAmount <= 0.0f)
	{
		return;
	}

	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator);
	if (!CheonbokCharacter || CheonbokCharacter->IsDead())
	{
		return;
	}

	const float PreviousHealth = CheonbokCharacter->GetHealth();
	if (PreviousHealth >= CheonbokCharacter->GetMaxHealth())
	{
		UE_LOG(
			LogTemp,
			Verbose,
			TEXT("%s cannot collect %s because health is already full."),
			*GetNameSafe(Activator),
			*ItemType.ToString());
		return;
	}

	CheonbokCharacter->AddHealth(HealingAmount);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s recovered %.1f health from %s."),
		*GetNameSafe(Activator),
		CheonbokCharacter->GetHealth() - PreviousHealth,
		*ItemType.ToString());

	Super::ActivateItem_Implementation(Activator);
}
