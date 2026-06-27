// Copyright 2026 liomwh-wyatt. All Rights Reserved.


#include "UI/Ch03_MainMenuWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/Ch03_GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UCh03_MainMenuWidget::UCh03_MainMenuWidget(
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

void UCh03_MainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bTravelRequested = false;

	if (StartButton)
	{
		StartButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_MainMenuWidget::HandleStartClicked);
		StartButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_MainMenuWidget::HandleButtonHovered);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_MainMenuWidget::HandleQuitClicked);
		QuitButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_MainMenuWidget::HandleButtonHovered);
	}

	UpdateHighScoreText();
}

void UCh03_MainMenuWidget::NativeDestruct()
{
	UnbindButtons();
	Super::NativeDestruct();
}

UWidget* UCh03_MainMenuWidget::GetInitialFocusWidget() const
{
	return StartButton && StartButton->GetIsFocusable()
		? StartButton
		: nullptr;
}

void UCh03_MainMenuWidget::HandleStartClicked()
{
	if (bTravelRequested)
	{
		return;
	}

	PlayButtonClickSound();

	UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>();

	if (!CheonbokGameInstance)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Cannot start Cheonbok Land: Ch03_GameInstance is missing."));
		return;
	}

	const FName FirstLevelName =
		CheonbokGameInstance->GetFirstLevelName();

	if (!CheonbokGameInstance->IsLevelAvailable(FirstLevelName))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Cannot start Cheonbok Land: first level %s is unavailable."),
			*FirstLevelName.ToString());
		return;
	}

	bTravelRequested = true;
	SetButtonsEnabled(false);
	CheonbokGameInstance->StartNewGame();

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, FirstLevelName);
}

void UCh03_MainMenuWidget::HandleQuitClicked()
{
	PlayButtonClickSound();
	SetButtonsEnabled(false);

	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}

void UCh03_MainMenuWidget::UpdateHighScoreText()
{
	if (!HighScoreText)
	{
		return;
	}

	const UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>();

	const int32 HighestScore = CheonbokGameInstance
		? CheonbokGameInstance->GetHighestScore()
		: 0;

	HighScoreText->SetText(
		FText::Format(
			NSLOCTEXT(
				"CheonbokMainMenu",
				"HighScoreFormat",
				"최고 간식 점수: {0}"),
			FText::AsNumber(HighestScore)));
}

void UCh03_MainMenuWidget::UnbindButtons()
{
	if (StartButton)
	{
		StartButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_MainMenuWidget::HandleStartClicked);
		StartButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_MainMenuWidget::HandleButtonHovered);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_MainMenuWidget::HandleQuitClicked);
		QuitButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_MainMenuWidget::HandleButtonHovered);
	}
}

void UCh03_MainMenuWidget::SetButtonsEnabled(
	const bool bEnabled)
{
	if (StartButton)
	{
		StartButton->SetIsEnabled(bEnabled);
	}

	if (QuitButton)
	{
		QuitButton->SetIsEnabled(bEnabled);
	}
}

void UCh03_MainMenuWidget::HandleButtonHovered()
{
	PlayUISound(ButtonHoverSound);
}

void UCh03_MainMenuWidget::PlayUISound(USoundBase* Sound) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			Sound,
			UISoundVolumeMultiplier);
	}
}

void UCh03_MainMenuWidget::PlayButtonClickSound() const
{
	PlayUISound(ButtonClickSound);
}

