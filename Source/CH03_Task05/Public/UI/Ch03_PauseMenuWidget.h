// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_PauseMenuWidget.generated.h"

class UButton;
class UWidget;

UCLASS()
class CH03_TASK05_API UCh03_PauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCh03_PauseMenuWidget(const FObjectInitializer& ObjectInitializer);

	UWidget* GetInitialFocusWidget() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleMainMenuClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> MainMenuButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;

private:
	void BuildNativeFallbackWidget();
	UButton* CreateNativeButton(const FText& Label, FName ButtonName);
	void BindButtons();
	void UnbindButtons();
};
