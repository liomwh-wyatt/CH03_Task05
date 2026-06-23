// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_GameResultWidget.generated.h"

class UButton;
class UTextBlock;
class UWidget;

UCLASS()
class CH03_TASK05_API UCh03_GameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Result")
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

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Result")
	void OnResultInitialized(
		bool bWasVictory,
		int32 FinalScore,
		bool bCanContinue);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Result")
	void OnHighScoreEvaluated(
		int32 HighestScore,
		bool bIsNewHighScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Result")
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result")
	FName MainMenuLevelName = TEXT("L_MainMenu");

private:
	void UnbindButtons();
	void ResumeGameBeforeTravel();

	FName CurrentLevelName = NAME_None;
	FName NextLevelName = NAME_None;
	bool bResultWasVictory = false;
};
