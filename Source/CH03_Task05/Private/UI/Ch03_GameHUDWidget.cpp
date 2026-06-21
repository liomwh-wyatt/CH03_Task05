// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameHUDWidget.h"

#include "Components/TextBlock.h"
#include "Core/Ch03_GameStateBase.h"

void UCh03_GameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindToGameState();
}

void UCh03_GameHUDWidget::NativeDestruct()
{
	if (BoundGameState)
	{
		BoundGameState->OnScoreChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleScoreChanged);
	}

	BoundGameState = nullptr;
	Super::NativeDestruct();
}

void UCh03_GameHUDWidget::HandleScoreChanged(int32 NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"ScoreFormat",
					"Snack Score: {0}"),
				FText::AsNumber(NewScore)));
	}

	OnScoreUpdated(NewScore);
}

void UCh03_GameHUDWidget::BindToGameState()
{
	BoundGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr;

	if (!BoundGameState)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GameHUDWidget could not find Ch03_GameStateBase."));
		return;
	}

	BoundGameState->OnScoreChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleScoreChanged);

	HandleScoreChanged(BoundGameState->GetScore());
}
