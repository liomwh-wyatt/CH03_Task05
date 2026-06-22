// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Ch03_GameHUDWidget.generated.h"

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

	UFUNCTION()
	void HandleWaveChanged(int32 CurrentWave, int32 MaxWave);

	UFUNCTION()
	void HandleRemainingTimeChanged(int32 NewRemainingTime);

	UFUNCTION()
	void HandleAnnouncementChanged(FText NewAnnouncement);

	UFUNCTION()
	void HandleStatusEffectChanged(
		ECheonbokStatusEffect EffectType,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnScoreUpdated(int32 NewScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnHealthUpdated(
		float CurrentHealth,
		float MaxHealth,
		float HealthPercent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnWaveUpdated(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnRemainingTimeUpdated(int32 NewRemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnAnnouncementUpdated(const FText& NewAnnouncement, bool bIsVisible);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnStatusEffectUpdated(
		ECheonbokStatusEffect EffectType,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveBannerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SlowStatusText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReverseControlStatusText;

private:
	void BindToGameState();
	void UnbindFromGameState();
	void BindToCharacter();
	void UnbindFromCharacter();
	void CreateStatusEffectTextFallbacks();
	void RefreshStatusEffectTexts();
	void UpdateStatusEffectText(
		UTextBlock* TargetText,
		const FText& Label,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime);

	UPROPERTY()
	TObjectPtr<ACh03_GameStateBase> BoundGameState;

	UPROPERTY()
	TObjectPtr<ACh03_CheonbokCharacter> BoundCharacter;

	FTimerHandle CharacterBindRetryTimerHandle;
	FTimerHandle StatusEffectRefreshTimerHandle;
};
