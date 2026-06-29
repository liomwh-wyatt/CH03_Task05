// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_GameResultWidget.generated.h"

class UButton;
class USoundBase;
class UTextBlock;
class UWidget;

UCLASS()
class CH03_TASK05_API UCh03_GameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCh03_GameResultWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "천복|결과")
	void InitializeResult(
		bool bWasVictory,
		int32 FinalScore,
		FName InNextLevelName,
		const FText& LevelDisplayName,
		int32 BestComboCount = 0);

	UWidget* GetInitialFocusWidget() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleRestartClicked();

	UFUNCTION()
	void HandleContinueClicked();

	UFUNCTION()
	void HandleMainMenuClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UFUNCTION()
	void HandleButtonHovered();

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|결과")
	void OnResultInitialized(
		bool bWasVictory,
		int32 FinalScore,
		bool bCanContinue);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|결과")
	void OnHighScoreEvaluated(
		int32 HighestScore,
		bool bIsNewHighScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|결과")
	void OnBestComboEvaluated(int32 BestComboCount);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultTitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FinalScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HighScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NewHighScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BestComboText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> MainMenuButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|결과", meta = (DisplayName = "메인 메뉴 레벨 이름"))
	FName MainMenuLevelName = TEXT("L_MainMenu");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "버튼 호버 사운드"))
	TObjectPtr<USoundBase> ButtonHoverSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "버튼 클릭 사운드"))
	TObjectPtr<USoundBase> ButtonClickSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드",
		meta = (DisplayName = "화면 사운드 볼륨 배율", ClampMin = "0.0"))
	float UISoundVolumeMultiplier = 0.9f;

private:
	void UnbindButtons();
	void ResumeGameBeforeTravel();
	void PlayUISound(USoundBase* Sound) const;
	void PlayButtonClickSound() const;

	FName CurrentLevelName = NAME_None;
	FName NextLevelName = NAME_None;
	bool bResultWasVictory = false;
};
