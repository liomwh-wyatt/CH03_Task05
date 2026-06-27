// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Ch03_GameStateBase.generated.h"

class USoundBase;

UENUM(BlueprintType)
enum class ECh03ComboBreakReason : uint8
{
	Manual		UMETA(DisplayName = "수동"),
	Timeout		UMETA(DisplayName = "시간 초과"),
	Hazard		UMETA(DisplayName = "위험 요소"),
	GameFlow	UMETA(DisplayName = "게임 진행")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCheonbokScoreChanged,
	int32, NewScore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnCheonbokComboChanged,
	int32, ComboCount,
	float, ComboTimeRemaining,
	float, ScoreMultiplier,
	bool, bIsActive);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCheonbokComboRewardTriggered,
	int32, ComboCount,
	FText, RewardText);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCheonbokGoldenItemRequested,
	int32, ComboCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCheonbokComboBroken,
	int32, PreviousComboCount,
	ECh03ComboBreakReason, BreakReason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCheonbokWaveChanged,
	int32, CurrentWave,
	int32, MaxWave);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCheonbokRemainingTimeChanged,
	int32, NewRemainingTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCheonbokAnnouncementChanged,
	FText, NewAnnouncement);

UCLASS()
class CH03_TASK05_API ACh03_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACh03_GameStateBase();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Score")
	int32 GetScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Score")
	int32 AddComboScore(int32 BaseAmount, AActor* ScoringActor);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Score")
	void ResetScore();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Score")
	void SetScore(int32 NewScore);

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave")
	int32 GetMaxWave() const { return MaxWave; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave")
	int32 GetRemainingTime() const { return RemainingTime; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave")
	int32 GetWaveDuration() const { return WaveDuration; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave")
	void SetWave(int32 NewCurrentWave, int32 NewMaxWave);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave")
	void SetRemainingTime(int32 NewRemainingTime);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave")
	void SetWaveDuration(int32 NewWaveDuration);

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Game Flow")
	FText GetAnnouncementText() const { return AnnouncementText; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Game Flow")
	void SetAnnouncementText(const FText& NewAnnouncement);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Game Flow")
	void ClearAnnouncementText();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Combo")
	void BreakCombo();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Combo")
	void BreakComboWithReason(ECh03ComboBreakReason BreakReason);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Combo")
	void ResetComboStats();

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Combo")
	int32 GetComboCount() const { return CurrentComboCount; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Combo")
	int32 GetBestComboCount() const { return BestComboCount; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Combo")
	float GetComboTimeRemaining() const { return ComboTimeRemaining; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Combo")
	float GetComboWindowSeconds() const { return ComboWindowSeconds; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Combo")
	float GetCurrentComboScoreMultiplier() const
	{
		return CurrentComboScoreMultiplier;
	}

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokComboChanged OnComboChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokComboRewardTriggered OnComboRewardTriggered;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokGoldenItemRequested OnGoldenItemRequested;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokComboBroken OnComboBroken;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokWaveChanged OnWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokRemainingTimeChanged OnRemainingTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokAnnouncementChanged OnAnnouncementChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Score")
	int32 CurrentScore = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo",
		meta = (ClampMin = "0.1", Units = "s"))
	float ComboWindowSeconds = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Combo12GoldenItemChance = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Combo")
	int32 CurrentComboCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Combo")
	int32 BestComboCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Combo")
	float ComboTimeRemaining = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Combo")
	float CurrentComboScoreMultiplier = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 CurrentWave = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 MaxWave = 3;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 RemainingTime = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 WaveDuration = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Game Flow")
	FText AnnouncementText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Audio")
	TObjectPtr<USoundBase> ComboRewardSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Audio")
	TObjectPtr<USoundBase> ComboBreakSound;

private:
	float GetScoreMultiplierForCombo(int32 ComboCount) const;
	void ProcessComboRewards(int32 ComboCount, AActor* ScoringActor);
	void ResetComboState(
		bool bShouldBroadcast,
		ECh03ComboBreakReason BreakReason,
		bool bShouldBroadcastBreak);
	void BroadcastComboChanged();
	void PlayUISound(USoundBase* Sound) const;

	bool bNextScoreItemDouble = false;
	TSet<int32> RewardedComboThresholds;
};
