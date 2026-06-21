// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Core/Ch03_GameStateBase.h"

ACh03_GameStateBase::ACh03_GameStateBase()
{
	CurrentScore = 0;
}

void ACh03_GameStateBase::AddScore(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok score: %d (+%d)"),
		CurrentScore,
		Amount);
}

void ACh03_GameStateBase::ResetScore()
{
	CurrentScore = 0;
	OnScoreChanged.Broadcast(CurrentScore);
}
