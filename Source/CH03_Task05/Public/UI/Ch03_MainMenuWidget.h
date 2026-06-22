// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_MainMenuWidget.generated.h"

class UButton;
class UWidget;

UCLASS()
class CH03_TASK05_API UCh03_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidget* GetInitialFocusWidget() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;

private:
	void UnbindButtons();
	void SetButtonsEnabled(bool bEnabled);

	bool bTravelRequested = false;
};
