// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_PauseMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Core/Ch03_CheonbokController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UCh03_PauseMenuWidget::UCh03_PauseMenuWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);

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

TSharedRef<SWidget> UCh03_PauseMenuWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		BuildNativeFallbackWidget();
	}

	return Super::RebuildWidget();
}

void UCh03_PauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindButtons();
}

void UCh03_PauseMenuWidget::NativeDestruct()
{
	UnbindButtons();

	Super::NativeDestruct();
}

UWidget* UCh03_PauseMenuWidget::GetInitialFocusWidget() const
{
	return ResumeButton && ResumeButton->GetIsFocusable()
		? ResumeButton
		: nullptr;
}

void UCh03_PauseMenuWidget::HandleResumeClicked()
{
	PlayButtonClickSound();

	if (ACh03_CheonbokController* CheonbokController =
		GetOwningPlayer<ACh03_CheonbokController>())
	{
		CheonbokController->ClosePauseMenu();
	}
}

void UCh03_PauseMenuWidget::HandleMainMenuClicked()
{
	PlayButtonClickSound();

	if (ACh03_CheonbokController* CheonbokController =
		GetOwningPlayer<ACh03_CheonbokController>())
	{
		CheonbokController->ReturnToMainMenuFromPause();
	}
}

void UCh03_PauseMenuWidget::HandleQuitClicked()
{
	PlayButtonClickSound();

	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}

void UCh03_PauseMenuWidget::BuildNativeFallbackWidget()
{
	if (!WidgetTree)
	{
		return;
	}

	UCanvasPanel* RootCanvas =
		WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("PauseRootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	UBorder* BackgroundPanel =
		WidgetTree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			TEXT("PauseBackgroundPanel"));
	BackgroundPanel->SetBrushColor(
		FLinearColor(0.02f, 0.018f, 0.03f, 0.88f));
	BackgroundPanel->SetPadding(FMargin(32.0f));

	UCanvasPanelSlot* BackgroundSlot =
		RootCanvas->AddChildToCanvas(BackgroundPanel);
	BackgroundSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	BackgroundSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	BackgroundSlot->SetSize(FVector2D(440.0f, 360.0f));

	UVerticalBox* MenuBox =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("PauseMenuBox"));
	BackgroundPanel->SetContent(MenuBox);

	UTextBlock* TitleText =
		WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			TEXT("PauseTitleText"));
	TitleText->SetText(
		NSLOCTEXT(
			"CheonbokPause",
			"PauseTitle",
			"Cheonbok Land Paused"));
	TitleText->SetJustification(ETextJustify::Center);
	TitleText->SetColorAndOpacity(
		FSlateColor(FLinearColor(1.0f, 0.92f, 0.55f, 1.0f)));

	UVerticalBoxSlot* TitleSlot =
		MenuBox->AddChildToVerticalBox(TitleText);
	TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 24.0f));

	ResumeButton = CreateNativeButton(
		NSLOCTEXT(
			"CheonbokPause",
			"ResumeButton",
			"Resume"),
		TEXT("ResumeButton"));
	MainMenuButton = CreateNativeButton(
		NSLOCTEXT(
			"CheonbokPause",
			"MainMenuButton",
			"Main Menu"),
		TEXT("MainMenuButton"));
	QuitButton = CreateNativeButton(
		NSLOCTEXT(
			"CheonbokPause",
			"QuitButton",
			"Quit"),
		TEXT("QuitButton"));

	for (UButton* Button : { ResumeButton, MainMenuButton, QuitButton })
	{
		if (!Button)
		{
			continue;
		}

		USizeBox* ButtonSizeBox =
			WidgetTree->ConstructWidget<USizeBox>(
				USizeBox::StaticClass());
		ButtonSizeBox->SetHeightOverride(54.0f);
		ButtonSizeBox->SetContent(Button);

		UVerticalBoxSlot* ButtonSlot =
			MenuBox->AddChildToVerticalBox(ButtonSizeBox);
		ButtonSlot->SetPadding(FMargin(0.0f, 8.0f));
	}
}

UButton* UCh03_PauseMenuWidget::CreateNativeButton(
	const FText& Label,
	const FName ButtonName)
{
	if (!WidgetTree)
	{
		return nullptr;
	}

	UButton* Button =
		WidgetTree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			ButtonName);

	UTextBlock* ButtonText =
		WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass());
	ButtonText->SetText(Label);
	ButtonText->SetJustification(ETextJustify::Center);
	ButtonText->SetColorAndOpacity(
		FSlateColor(FLinearColor::White));

	Button->SetContent(ButtonText);

	return Button;
}

void UCh03_PauseMenuWidget::BindButtons()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleResumeClicked);
		ResumeButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleButtonHovered);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleMainMenuClicked);
		MainMenuButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleButtonHovered);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleQuitClicked);
		QuitButton->OnHovered.AddUniqueDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleButtonHovered);
	}
}

void UCh03_PauseMenuWidget::UnbindButtons()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleResumeClicked);
		ResumeButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleButtonHovered);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleMainMenuClicked);
		MainMenuButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleButtonHovered);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleQuitClicked);
		QuitButton->OnHovered.RemoveDynamic(
			this,
			&UCh03_PauseMenuWidget::HandleButtonHovered);
	}
}

void UCh03_PauseMenuWidget::HandleButtonHovered()
{
	PlayUISound(ButtonHoverSound);
}

void UCh03_PauseMenuWidget::PlayUISound(USoundBase* Sound) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			Sound,
			UISoundVolumeMultiplier);
	}
}

void UCh03_PauseMenuWidget::PlayButtonClickSound() const
{
	PlayUISound(ButtonClickSound);
}
