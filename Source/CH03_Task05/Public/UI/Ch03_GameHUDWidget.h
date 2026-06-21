// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Ch03_GameHUDWidget.generated.h"

class ACh03_CheonbokCharacter;
class ACh03_GameStateBase;
class UProgressBar;
class UTextBlock;

UCLASS()
class CH03_TASK05_API UCh03_GameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleScoreChanged(int32 NewScore);

	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnScoreUpdated(int32 NewScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnHealthUpdated(
		float CurrentHealth,
		float MaxHealth,
		float HealthPercent);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

private:
	void BindToGameState();
	void BindToCharacter();
	void UnbindFromCharacter();

	UPROPERTY()
	TObjectPtr<ACh03_GameStateBase> BoundGameState;

	UPROPERTY()
	TObjectPtr<ACh03_CheonbokCharacter> BoundCharacter;

	FTimerHandle CharacterBindRetryTimerHandle;
};
