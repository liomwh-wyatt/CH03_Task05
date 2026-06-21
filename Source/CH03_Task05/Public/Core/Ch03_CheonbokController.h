#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Ch03_CheonbokController.generated.h"

class UInputAction;
class UInputMappingContext;
class UCh03_GameHUDWidget;

UCLASS()
class CH03_TASK05_API ACh03_CheonbokController : public APlayerController
{
	GENERATED_BODY()

public:
	ACh03_CheonbokController();

	bool IsSprintInputHeld() const;

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

protected:
	virtual void BeginPlay() override;

private:
	void AddDefaultMappingContext();
	void CreateGameHUD();
};
