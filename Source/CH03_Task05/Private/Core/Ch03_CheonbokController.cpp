#include "Core/Ch03_CheonbokController.h"

#include "Components/InputComponent.h"
#include "Core/Ch03_GameInstance.h"
#include "Cutscene/Ch03_CutsceneDirector.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputCoreTypes.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Ch03_GameHUDWidget.h"
#include "UI/Ch03_PauseMenuWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ACh03_CheonbokController::ACh03_CheonbokController()
{
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	LookAction = nullptr;
	JumpAction = nullptr;
	SprintAction = nullptr;
	GameHUDWidgetClass = nullptr;
	GameHUDWidget = nullptr;
	PauseMenuWidgetClass = UCh03_PauseMenuWidget::StaticClass();
	PauseMenuWidget = nullptr;
	ActiveCutsceneDirector = nullptr;
	bSprintInputHeld = false;
	bShouldShowGameHUD = true;
}

void ACh03_CheonbokController::BeginPlay()
{
	Super::BeginPlay();

	AddDefaultMappingContext();
	CreateGameHUD();
}

void ACh03_CheonbokController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindKey(
		EKeys::Escape,
		IE_Pressed,
		this,
		&ACh03_CheonbokController::TogglePauseMenu);

	InputComponent->BindKey(
		EKeys::P,
		IE_Pressed,
		this,
		&ACh03_CheonbokController::TogglePauseMenu);

	InputComponent->BindKey(
		EKeys::Gamepad_Special_Right,
		IE_Pressed,
		this,
		&ACh03_CheonbokController::TogglePauseMenu);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(
				SprintAction,
				ETriggerEvent::Started,
				this,
				&ACh03_CheonbokController::HandleSprintStarted);
			EnhancedInputComponent->BindAction(
				SprintAction,
				ETriggerEvent::Completed,
				this,
				&ACh03_CheonbokController::HandleSprintStopped);
			EnhancedInputComponent->BindAction(
				SprintAction,
				ETriggerEvent::Canceled,
				this,
				&ACh03_CheonbokController::HandleSprintStopped);
		}
	}
}

bool ACh03_CheonbokController::IsSprintInputHeld() const
{
	if (!SprintAction)
	{
		return false;
	}

	const UEnhancedPlayerInput* EnhancedPlayerInput =
		Cast<UEnhancedPlayerInput>(PlayerInput);

	if (EnhancedPlayerInput
		&& !EnhancedPlayerInput->GetActionValue(SprintAction).Get<bool>())
	{
		return false;
	}

	return bSprintInputHeld;
}

void ACh03_CheonbokController::AddDefaultMappingContext()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!DefaultMappingContext)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("DefaultMappingContext is not assigned on CheonbokController."));
		return;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return;
	}

	InputSubsystem->RemoveMappingContext(DefaultMappingContext);
	InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
}

void ACh03_CheonbokController::CreateGameHUD()
{
	if (!IsLocalController() || !GameHUDWidgetClass || GameHUDWidget)
	{
		return;
	}

	GameHUDWidget = CreateWidget<UCh03_GameHUDWidget>(
		this,
		GameHUDWidgetClass);

	if (GameHUDWidget)
	{
		GameHUDWidget->AddToViewport();
		ApplyGameHUDVisibility();
	}
}

void ACh03_CheonbokController::TogglePauseMenu()
{
	if (SkipActiveCutscene())
	{
		return;
	}

	if (PauseMenuWidget)
	{
		ClosePauseMenu();
		return;
	}

	OpenPauseMenu();
}

void ACh03_CheonbokController::OpenPauseMenu()
{
	if (!IsLocalController() || PauseMenuWidget)
	{
		return;
	}

	if (UGameplayStatics::IsGamePaused(this))
	{
		UE_LOG(
			LogTemp,
			Verbose,
			TEXT("Pause menu ignored because the game is already paused by another flow."));
		return;
	}

	if (!PauseMenuWidgetClass)
	{
		PauseMenuWidgetClass = UCh03_PauseMenuWidget::StaticClass();
	}

	PauseMenuWidget = CreateWidget<UCh03_PauseMenuWidget>(
		this,
		PauseMenuWidgetClass);

	if (!PauseMenuWidget)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Failed to create Ch03_PauseMenuWidget."));
		return;
	}

	PauseMenuWidget->AddToViewport(90);
	UGameplayStatics::SetGamePaused(this, true);
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	ApplyPauseInputMode();
}

void ACh03_CheonbokController::ClosePauseMenu()
{
	if (!PauseMenuWidget)
	{
		return;
	}

	PauseMenuWidget->RemoveFromParent();
	PauseMenuWidget = nullptr;

	UGameplayStatics::SetGamePaused(this, false);
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	ApplyGameplayInputMode();
}

void ACh03_CheonbokController::ReturnToMainMenuFromPause()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}

	if (UCh03_GameInstance* CheonbokGameInstance =
		GetGameInstance<UCh03_GameInstance>())
	{
		CheonbokGameInstance->ResetProgress();
	}

	UGameplayStatics::SetGamePaused(this, false);
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	ApplyGameplayInputMode();

	if (!MainMenuLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, MainMenuLevelName);
	}
}

void ACh03_CheonbokController::RegisterActiveCutsceneDirector(
	ACh03_CutsceneDirector* CutsceneDirector)
{
	if (!CutsceneDirector)
	{
		return;
	}

	ActiveCutsceneDirector = CutsceneDirector;
}

void ACh03_CheonbokController::ClearActiveCutsceneDirector(
	ACh03_CutsceneDirector* CutsceneDirector)
{
	if (!CutsceneDirector || ActiveCutsceneDirector == CutsceneDirector)
	{
		ActiveCutsceneDirector = nullptr;
	}
}

bool ACh03_CheonbokController::SkipActiveCutscene()
{
	if (!ActiveCutsceneDirector
		|| !ActiveCutsceneDirector->IsCutscenePlaying())
	{
		return false;
	}

	ActiveCutsceneDirector->SkipCutscene();
	return true;
}

void ACh03_CheonbokController::SetGameHUDVisible(
	const bool bIsVisible)
{
	bShouldShowGameHUD = bIsVisible;
	CreateGameHUD();
	ApplyGameHUDVisibility();
}

void ACh03_CheonbokController::ApplyGameHUDVisibility()
{
	const ESlateVisibility TargetVisibility =
		bShouldShowGameHUD
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed;

	if (GameHUDWidget)
	{
		GameHUDWidget->SetVisibility(TargetVisibility);
	}

	TArray<UUserWidget*> HUDWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		this,
		HUDWidgets,
		UCh03_GameHUDWidget::StaticClass(),
		false);

	for (UUserWidget* HUDWidget : HUDWidgets)
	{
		if (HUDWidget)
		{
			HUDWidget->SetVisibility(TargetVisibility);
		}
	}
}

void ACh03_CheonbokController::ApplyGameplayInputMode()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ACh03_CheonbokController::ApplyPauseInputMode()
{
	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	if (PauseMenuWidget)
	{
		if (UWidget* InitialFocusWidget =
			PauseMenuWidget->GetInitialFocusWidget())
		{
			InputMode.SetWidgetToFocus(
				InitialFocusWidget->TakeWidget());
		}
	}
	InputMode.SetLockMouseToViewportBehavior(
		EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);
}

void ACh03_CheonbokController::HandleSprintStarted()
{
	bSprintInputHeld = true;
}

void ACh03_CheonbokController::HandleSprintStopped()
{
	bSprintInputHeld = false;
}
