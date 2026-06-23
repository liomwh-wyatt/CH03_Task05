#include "Items/Ch03_StaminaRecoveryItem.h"

#include "Character/Ch03_CheonbokCharacter.h"

ACh03_StaminaRecoveryItem::ACh03_StaminaRecoveryItem()
{
	ItemType = TEXT("StaminaRecoveryItem");
	RotationSpeed = 100.0f;
	BobAmplitude = 14.0f;
	BobFrequency = 1.7f;
	LifetimeAfterSpawn = 12.0f;
}

void ACh03_StaminaRecoveryItem::ActivateItem_Implementation(
	AActor* Activator)
{
	if (IsConsumed()
		|| !CanBeActivatedBy(Activator)
		|| StaminaRecoveryAmount <= 0.0f)
	{
		return;
	}

	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator);
	if (!CheonbokCharacter || CheonbokCharacter->IsDead())
	{
		return;
	}

	const float PreviousStamina = CheonbokCharacter->GetStamina();
	if (PreviousStamina >= CheonbokCharacter->GetMaxStamina())
	{
		UE_LOG(
			LogTemp,
			Verbose,
			TEXT("%s cannot collect %s because stamina is already full."),
			*GetNameSafe(Activator),
			*ItemType.ToString());
		return;
	}

	CheonbokCharacter->AddStamina(StaminaRecoveryAmount);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s recovered %.1f stamina from %s."),
		*GetNameSafe(Activator),
		CheonbokCharacter->GetStamina() - PreviousStamina,
		*ItemType.ToString());

	Super::ActivateItem_Implementation(Activator);
}
