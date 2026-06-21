#include "Core/Ch03_CheonbokController.h"

#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "UI/Ch03_GameHUDWidget.h"

ACh03_CheonbokController::ACh03_CheonbokController()
{
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	LookAction = nullptr;
	JumpAction = nullptr;
	SprintAction = nullptr;
	GameHUDWidgetClass = nullptr;
	GameHUDWidget = nullptr;
}

void ACh03_CheonbokController::BeginPlay()
{
	Super::BeginPlay();

	AddDefaultMappingContext();
	CreateGameHUD();
}

bool ACh03_CheonbokController::IsSprintInputHeld() const
{
	if (!SprintAction)
	{
		return false;
	}

	const UEnhancedPlayerInput* EnhancedPlayerInput =
		Cast<UEnhancedPlayerInput>(PlayerInput);

	return EnhancedPlayerInput
		&& EnhancedPlayerInput->GetActionValue(SprintAction).Get<bool>();
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
	}
}
