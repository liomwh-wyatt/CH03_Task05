#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Ch03_CheonbokController.generated.h"

class UInputAction;
class UInputMappingContext;
class UCh03_GameHUDWidget;
class UCh03_PauseMenuWidget;

UCLASS()
class CH03_TASK05_API ACh03_CheonbokController : public APlayerController
{
	GENERATED_BODY()

public:
	ACh03_CheonbokController();

	bool IsSprintInputHeld() const;

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Pause")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Pause")
	void OpenPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Pause")
	void ClosePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Pause")
	void ReturnToMainMenuFromPause();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|UI")
	TSubclassOf<UCh03_GameHUDWidget> GameHUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Cheonbok|UI")
	TObjectPtr<UCh03_GameHUDWidget> GameHUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|UI")
	TSubclassOf<UCh03_PauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Cheonbok|UI")
	TObjectPtr<UCh03_PauseMenuWidget> PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|UI")
	FName MainMenuLevelName = TEXT("L_MainMenu");

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void AddDefaultMappingContext();
	void CreateGameHUD();
	void ApplyGameplayInputMode();
	void ApplyPauseInputMode();
};
