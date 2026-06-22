// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Core/Ch03_MainMenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Core/Ch03_GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Ch03_MainMenuWidget.h"
#include "UObject/ConstructorHelpers.h"

ACh03_MainMenuGameMode::ACh03_MainMenuGameMode()
{
	DefaultPawnClass = nullptr;
	SpectatorClass = nullptr;
	HUDClass = nullptr;
	bStartPlayersAsSpectators = true;

	static ConstructorHelpers::FClassFinder<UCh03_MainMenuWidget> MainMenuWidgetFinder(
		TEXT("/Game/UI/WBP_MainMenu"));

	if (MainMenuWidgetFinder.Succeeded())
	{
		MainMenuWidgetClass = MainMenuWidgetFinder.Class;
	}
}

void ACh03_MainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>())
	{
		CheonbokGameInstance->ResetProgress();
	}

	APlayerController* PlayerController =
		UGameplayStatics::GetPlayerController(this, 0);

	if (!PlayerController)
	{
		return;
	}

	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);

	if (!MainMenuWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MainMenuWidgetClass is not set on Ch03_MainMenuGameMode."));
		return;
	}

	ActiveMainMenuWidget =
		CreateWidget<UCh03_MainMenuWidget>(
			PlayerController,
			MainMenuWidgetClass);

	if (!ActiveMainMenuWidget)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Failed to create Ch03_MainMenuWidget."));
		return;
	}

	ActiveMainMenuWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	if (UWidget* FocusWidget =
		ActiveMainMenuWidget->GetInitialFocusWidget())
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
}

void ACh03_MainMenuGameMode::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	if (ActiveMainMenuWidget)
	{
		ActiveMainMenuWidget->RemoveFromParent();
		ActiveMainMenuWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}
