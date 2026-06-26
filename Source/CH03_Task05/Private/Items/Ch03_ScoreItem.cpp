#include "Items/Ch03_ScoreItem.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Core/Ch03_GameStateBase.h"
#include "Feedback/Ch03_FeedbackCue.h"

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

	const int32 FinalScore =
		CheonbokGameState->AddComboScore(ScoreValue, Activator);

	bool bPlayedCheonbokFeedback = false;
	if (ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator))
	{
		bPlayedCheonbokFeedback =
			CheonbokCharacter->PlayScorePickupFeedback(
				this,
				ScoreValue,
				FinalScore);
	}

	if (!bPlayedCheonbokFeedback)
	{
		UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtActor(
			this,
			ScorePickupFeedback,
			Activator);
	}

	OnScoreCollectedFeedback(Activator, ScoreValue, FinalScore);

	Super::ActivateItem_Implementation(Activator);
}
