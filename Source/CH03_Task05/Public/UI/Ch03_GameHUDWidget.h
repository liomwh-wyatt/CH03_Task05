// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Core/Ch03_GameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Ch03_GameHUDWidget.generated.h"

class ACh03_GameStateBase;
class UImage;
class UProgressBar;
class UTextBlock;
class UTextureRenderTarget2D;

UCLASS()
class CH03_TASK05_API UCh03_GameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime) override;

	UFUNCTION()
	void HandleScoreChanged(int32 NewScore);

	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleStaminaChanged(float CurrentStamina, float MaxStamina);

	UFUNCTION()
	void HandleWaveChanged(int32 CurrentWave, int32 MaxWave);

	UFUNCTION()
	void HandleRemainingTimeChanged(int32 NewRemainingTime);

	UFUNCTION()
	void HandleAnnouncementChanged(FText NewAnnouncement);

	UFUNCTION()
	void HandleComboChanged(
		int32 ComboCount,
		float ComboTimeRemaining,
		float ScoreMultiplier,
		bool bIsActive);

	UFUNCTION()
	void HandleComboRewardTriggered(
		int32 ComboCount,
		FText RewardText);

	UFUNCTION()
	void HandleComboBroken(
		int32 PreviousComboCount,
		ECh03ComboBreakReason BreakReason);

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
	void OnStaminaUpdated(
		float CurrentStamina,
		float MaxStamina,
		float StaminaPercent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnWaveUpdated(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnRemainingTimeUpdated(int32 NewRemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnAnnouncementUpdated(const FText& NewAnnouncement, bool bIsVisible);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnComboUpdated(
		int32 ComboCount,
		float ComboTimeRemaining,
		float ScoreMultiplier,
		bool bIsVisible);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnComboRewardUpdated(
		int32 ComboCount,
		const FText& RewardText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnComboBroken(
		int32 PreviousComboCount,
		ECh03ComboBreakReason BreakReason);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnStatusEffectUpdated(
		ECheonbokStatusEffect EffectType,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnPortraitRenderTargetUpdated(UTextureRenderTarget2D* RenderTarget);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StaminaText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> CheonbokFaceImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> TimerBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveBannerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ComboText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ComboRewardText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SlowStatusText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReverseControlStatusText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MovementLockStatusText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DamageShieldStatusText;

private:
	void BindToGameState();
	void UnbindFromGameState();
	void BindToCharacter();
	void UnbindFromCharacter();
	void CreateWaveBannerTextFallback();
	void CreateStaminaFallbacks();
	void CreateComboFallbacks();
	void PlayWaveBannerIntroAnimation();
	void PlayWaveBannerOutroAnimation();
	void UpdateWaveBannerAnimation(float DeltaTime);
	void ApplyWaveBannerAnimation(float Alpha, bool bIsOutro);
	void FinishWaveBannerOutroAnimation();
	void UpdateComboRewardFeedback(float DeltaTime);
	void UpdateTimerBarPercent(int32 SafeRemainingTime);
	void UpdateTimerBarDisplay();
	void ResetTimerBar();
	void InitializeTransientTextWidgets();
	void CreateStatusEffectTextFallbacks();
	void RefreshStatusEffectTexts();
	void RefreshPortraitImage();
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

	float WaveBannerAnimationTime = 0.0f;
	bool bIsWaveBannerAnimationActive = false;
	bool bIsWaveBannerOutroAnimation = false;
	float ComboRewardRemainingTime = 0.0f;
	float TimerBarDurationSeconds = 0.0f;
	float TimerBarEndTimeSeconds = 0.0f;
	int32 TimerBarMaxSeconds = 0;
	int32 LastObservedTimerWave = INDEX_NONE;
	bool bIsTimerBarCountingDown = false;

	static constexpr float WaveBannerIntroDuration = 0.22f;
	static constexpr float WaveBannerOutroDuration = 0.18f;
	static constexpr float ComboRewardDisplayDuration = 1.6f;
	static constexpr float ComboBreakDisplayDuration = 1.1f;
};
