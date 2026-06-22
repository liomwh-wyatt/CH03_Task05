// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/Ch03_GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UCh03_GameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleRestartClicked);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleContinueClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleQuitClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleMainMenuClicked);
	}
}

void UCh03_GameResultWidget::NativeDestruct()
{
	UnbindButtons();
	Super::NativeDestruct();
}

void UCh03_GameResultWidget::InitializeResult(
	const bool bWasVictory,
	const int32 FinalScore,
	const FName InNextLevelName,
	const FText& LevelDisplayName)
{
	CurrentLevelName = FName(
		*UGameplayStatics::GetCurrentLevelName(this, true));
	NextLevelName = InNextLevelName;
	bResultWasVictory = bWasVictory;

	int32 HighestScore = FMath::Max(0, FinalScore);
	bool bIsNewHighScore = false;

	if (UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>())
	{
		bIsNewHighScore =
			CheonbokGameInstance->SubmitScore(FinalScore);
		HighestScore =
			CheonbokGameInstance->GetHighestScore();
	}

	if (ResultTitleText)
	{
		ResultTitleText->SetText(
			bWasVictory
				? FText::Format(
					NSLOCTEXT(
						"CheonbokResult",
						"LevelComplete",
						"{0} Complete!"),
					LevelDisplayName)
				: NSLOCTEXT(
					"CheonbokResult",
					"GameOver",
					"Game Over"));
	}

	if (FinalScoreText)
	{
		FinalScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokResult",
					"FinalScore",
					"Final Snack Score: {0}"),
				FText::AsNumber(FMath::Max(0, FinalScore))));
	}

	if (HighScoreText)
	{
		HighScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokResult",
					"HighScore",
					"Best Snack Score: {0}"),
				FText::AsNumber(HighestScore)));
	}

	if (NewHighScoreText)
	{
		NewHighScoreText->SetText(
			NSLOCTEXT(
				"CheonbokResult",
				"NewHighScore",
				"New Best!"));
		NewHighScoreText->SetVisibility(
			bIsNewHighScore
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}

	const bool bCanContinue =
		bWasVictory && !NextLevelName.IsNone();

	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(bCanContinue);
		ContinueButton->SetVisibility(
			bWasVictory
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}

	OnResultInitialized(
		bWasVictory,
		FMath::Max(0, FinalScore),
		bCanContinue);

	OnHighScoreEvaluated(
		HighestScore,
		bIsNewHighScore);
}

UWidget* UCh03_GameResultWidget::GetInitialFocusWidget() const
{
	return RestartButton && RestartButton->GetIsFocusable()
		? RestartButton
		: nullptr;
}

void UCh03_GameResultWidget::HandleRestartClicked()
{
	FName RestartLevelName = CurrentLevelName;

	if (UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>())
	{
		if (bResultWasVictory)
		{
			CheonbokGameInstance->StartNewGame();
			RestartLevelName =
				CheonbokGameInstance->GetFirstLevelName();
		}
		else
		{
			CheonbokGameInstance->PrepareTravelToLevel(
				CurrentLevelName);
		}
	}

	if (RestartLevelName.IsNone())
	{
		return;
	}

	ResumeGameBeforeTravel();
	UGameplayStatics::OpenLevel(this, RestartLevelName);
}

void UCh03_GameResultWidget::HandleContinueClicked()
{
	if (NextLevelName.IsNone())
	{
		return;
	}

	if (UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>())
	{
		if (!CheonbokGameInstance->PrepareTravelToLevel(
			NextLevelName))
		{
			return;
		}
	}

	ResumeGameBeforeTravel();
	UGameplayStatics::OpenLevel(this, NextLevelName);
}

void UCh03_GameResultWidget::HandleMainMenuClicked()
{
	if (MainMenuLevelName.IsNone())
	{
		return;
	}

	if (UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>())
	{
		CheonbokGameInstance->ResetProgress();
	}

	ResumeGameBeforeTravel();
	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

void UCh03_GameResultWidget::HandleQuitClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}

void UCh03_GameResultWidget::UnbindButtons()
{
	if (RestartButton)
	{
		RestartButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleRestartClicked);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleContinueClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleQuitClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleMainMenuClicked);
	}
}

void UCh03_GameResultWidget::ResumeGameBeforeTravel()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}
