#include "Core/Ch03_CheonbokController.h"

#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"

ACh03_CheonbokController::ACh03_CheonbokController()
{
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	LookAction = nullptr;
	JumpAction = nullptr;
	SprintAction = nullptr;
}

void ACh03_CheonbokController::BeginPlay()
{
	Super::BeginPlay();

	AddDefaultMappingContext();
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

