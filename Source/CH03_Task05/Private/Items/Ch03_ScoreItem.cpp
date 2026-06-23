#include "Items/Ch03_ScoreItem.h"

#include "Core/Ch03_GameStateBase.h"

ACh03_ScoreItem::ACh03_ScoreItem()
{
	ItemType = TEXT("ScoreItem");
	LifetimeAfterSpawn = 16.0f;
	BlinkStartTime = 3.0f;
}

void ACh03_ScoreItem::ActivateItem_Implementation(AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator))
	{
		return;
	}

	ACh03_GameStateBase* CheonbokGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr;

	if (!CheonbokGameState)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("ScoreItem requires Ch03_GameStateBase."));
		return;
	}

	CheonbokGameState->AddScore(ScoreValue);
	Super::ActivateItem_Implementation(Activator);
}
