// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/Ch03_GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UCh03_GameResultWidget::UCh03_GameResultWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USoundBase> HoverSoundFinder(
		TEXT("/Game/Audio/UI/S_UI_Hover.S_UI_Hover"));
	if (HoverSoundFinder.Succeeded())
	{
		ButtonHoverSound = HoverSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> ClickSoundFinder(
		TEXT("/Game/Audio/UI/S_UI_Click.S_UI_Click"));
	if (ClickSoundFinder.Succeeded())
	{
		ButtonClickSound = ClickSoundFinder.Object;
	}
}

void UCh03_GameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleRestartClicked);
		RestartButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleContinueClicked);
		ContinueButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleQuitClicked);
		QuitButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleMainMenuClicked);
		MainMenuButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
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
	const FText& LevelDisplayName,
	const int32 BestComboCount)
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
						"{0} 클리어!"),
					LevelDisplayName)
				: NSLOCTEXT(
					"CheonbokResult",
					"GameOver",
					"게임 오버"));
	}

	if (FinalScoreText)
	{
		FinalScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokResult",
					"FinalScore",
					"최종 간식 점수: {0}"),
				FText::AsNumber(FMath::Max(0, FinalScore))));
	}

	if (HighScoreText)
	{
		HighScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokResult",
					"HighScore",
					"최고 간식 점수: {0}"),
				FText::AsNumber(HighestScore)));
	}

	if (NewHighScoreText)
	{
		NewHighScoreText->SetText(
			NSLOCTEXT(
				"CheonbokResult",
				"NewHighScore",
				"신기록!"));
		NewHighScoreText->SetVisibility(
			bIsNewHighScore
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}

	if (BestComboText)
	{
		BestComboText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokResult",
					"BestCombo",
					"최고 우다다 콤보: x{0}"),
				FText::AsNumber(FMath::Max(0, BestComboCount))));
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

	OnBestComboEvaluated(FMath::Max(0, BestComboCount));
}

UWidget* UCh03_GameResultWidget::GetInitialFocusWidget() const
{
	return RestartButton && RestartButton->GetIsFocusable()
		? RestartButton
		: nullptr;
}

void UCh03_GameResultWidget::HandleRestartClicked()
{
	PlayButtonClickSound();

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
	PlayButtonClickSound();

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
	PlayButtonClickSound();

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
	PlayButtonClickSound();

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
		RestartButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleContinueClicked);
		ContinueButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleQuitClicked);
		QuitButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleMainMenuClicked);
		MainMenuButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_GameResultWidget::HandleButtonHovered);
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

void UCh03_GameResultWidget::HandleButtonHovered()
{
	PlayUISound(ButtonHoverSound);
}

void UCh03_GameResultWidget::PlayUISound(USoundBase* Sound) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			Sound,
			UISoundVolumeMultiplier);
	}
}

void UCh03_GameResultWidget::PlayButtonClickSound() const
{
	PlayUISound(ButtonClickSound);
}
