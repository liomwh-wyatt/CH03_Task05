// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Core/Ch03_GameStateBase.h"

ACh03_GameStateBase::ACh03_GameStateBase()
{
	CurrentScore = 0;
	CurrentWave = 0;
	MaxWave = 3;
	RemainingTime = 0;
	AnnouncementText = FText::GetEmpty();
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

void ACh03_GameStateBase::SetWave(
	const int32 NewCurrentWave,
	const int32 NewMaxWave)
{
	const int32 SanitizedMaxWave = FMath::Max(1, NewMaxWave);
	const int32 SanitizedCurrentWave = FMath::Clamp(
		NewCurrentWave,
		0,
		SanitizedMaxWave);

	if (CurrentWave == SanitizedCurrentWave
		&& MaxWave == SanitizedMaxWave)
	{
		return;
	}

	CurrentWave = SanitizedCurrentWave;
	MaxWave = SanitizedMaxWave;
	OnWaveChanged.Broadcast(CurrentWave, MaxWave);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok wave: %d / %d"),
		CurrentWave,
		MaxWave);
}

void ACh03_GameStateBase::SetRemainingTime(
	const int32 NewRemainingTime)
{
	const int32 SanitizedRemainingTime = FMath::Max(
		0,
		NewRemainingTime);

	if (RemainingTime == SanitizedRemainingTime)
	{
		return;
	}

	RemainingTime = SanitizedRemainingTime;
	OnRemainingTimeChanged.Broadcast(RemainingTime);

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("Cheonbok remaining time: %d"),
		RemainingTime);
}

void ACh03_GameStateBase::SetAnnouncementText(
	const FText& NewAnnouncement)
{
	if (AnnouncementText.EqualTo(NewAnnouncement))
	{
		return;
	}

	AnnouncementText = NewAnnouncement;
	OnAnnouncementChanged.Broadcast(AnnouncementText);
}

void ACh03_GameStateBase::ClearAnnouncementText()
{
	SetAnnouncementText(FText::GetEmpty());
}
