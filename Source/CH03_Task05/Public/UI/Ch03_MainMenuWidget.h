// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_MainMenuWidget.generated.h"

class UButton;
class USoundBase;
class UTextBlock;
class UWidget;

UCLASS()
class CH03_TASK05_API UCh03_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCh03_MainMenuWidget(const FObjectInitializer& ObjectInitializer);

	UWidget* GetInitialFocusWidget() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UFUNCTION()
	void HandleButtonHovered();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HighScoreText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "버튼 호버 사운드"))
	TObjectPtr<USoundBase> ButtonHoverSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "버튼 클릭 사운드"))
	TObjectPtr<USoundBase> ButtonClickSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드",
		meta = (DisplayName = "화면 사운드 볼륨 배율", ClampMin = "0.0"))
	float UISoundVolumeMultiplier = 0.9f;

private:
	void UpdateHighScoreText();
	void UnbindButtons();
	void SetButtonsEnabled(bool bEnabled);
	void PlayUISound(USoundBase* Sound) const;
	void PlayButtonClickSound() const;

	bool bTravelRequested = false;
};
