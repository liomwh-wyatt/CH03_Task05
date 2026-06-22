// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
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
}

void UCh03_GameResultWidget::NativeDestruct()
{
	UnbindButtons();
	Super::NativeDestruct();
}

void UCh03_GameResultWidget::InitializeResult(
	const bool bWasVictory,
	const int32 FinalScore,
	const FName InNextLevelName)
{
	CurrentLevelName = FName(
		*UGameplayStatics::GetCurrentLevelName(this, true));
	NextLevelName = InNextLevelName;

	if (ResultTitleText)
	{
		ResultTitleText->SetText(
			bWasVictory
				? NSLOCTEXT(
					"CheonbokResult",
					"LivingRoomComplete",
					"Living Room Complete!")
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
}

void UCh03_GameResultWidget::HandleRestartClicked()
{
	if (CurrentLevelName.IsNone())
	{
		return;
	}

	ResumeGameBeforeTravel();
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}

void UCh03_GameResultWidget::HandleContinueClicked()
{
	if (NextLevelName.IsNone())
	{
		return;
	}

	ResumeGameBeforeTravel();
	UGameplayStatics::OpenLevel(this, NextLevelName);
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
