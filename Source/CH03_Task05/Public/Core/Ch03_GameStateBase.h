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

	UFUNCTION(BlueprintPure, Category = "천복|점수")
	int32 GetScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "천복|점수")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "천복|점수")
	int32 AddComboScore(int32 BaseAmount, AActor* ScoringActor);

	UFUNCTION(BlueprintCallable, Category = "천복|점수")
	void ResetScore();

	UFUNCTION(BlueprintCallable, Category = "천복|점수")
	void SetScore(int32 NewScore);

	UFUNCTION(BlueprintPure, Category = "천복|웨이브")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintPure, Category = "천복|웨이브")
	int32 GetMaxWave() const { return MaxWave; }

	UFUNCTION(BlueprintPure, Category = "천복|웨이브")
	int32 GetRemainingTime() const { return RemainingTime; }

	UFUNCTION(BlueprintPure, Category = "천복|웨이브")
	int32 GetWaveDuration() const { return WaveDuration; }

	UFUNCTION(BlueprintCallable, Category = "천복|웨이브")
	void SetWave(int32 NewCurrentWave, int32 NewMaxWave);

	UFUNCTION(BlueprintCallable, Category = "천복|웨이브")
	void SetRemainingTime(int32 NewRemainingTime);

	UFUNCTION(BlueprintCallable, Category = "천복|웨이브")
	void SetWaveDuration(int32 NewWaveDuration);

	UFUNCTION(BlueprintPure, Category = "천복|게임 진행")
	FText GetAnnouncementText() const { return AnnouncementText; }

	UFUNCTION(BlueprintCallable, Category = "천복|게임 진행")
	void SetAnnouncementText(const FText& NewAnnouncement);

	UFUNCTION(BlueprintCallable, Category = "천복|게임 진행")
	void ClearAnnouncementText();

	UFUNCTION(BlueprintCallable, Category = "천복|우다다 콤보")
	void BreakCombo();

	UFUNCTION(BlueprintCallable, Category = "천복|우다다 콤보")
	void BreakComboWithReason(ECh03ComboBreakReason BreakReason);

	UFUNCTION(BlueprintCallable, Category = "천복|우다다 콤보")
	void ResetComboStats();

	UFUNCTION(BlueprintPure, Category = "천복|우다다 콤보")
	int32 GetComboCount() const { return CurrentComboCount; }

	UFUNCTION(BlueprintPure, Category = "천복|우다다 콤보")
	int32 GetBestComboCount() const { return BestComboCount; }

	UFUNCTION(BlueprintPure, Category = "천복|우다다 콤보")
	float GetComboTimeRemaining() const { return ComboTimeRemaining; }

	UFUNCTION(BlueprintPure, Category = "천복|우다다 콤보")
	float GetComboWindowSeconds() const { return ComboWindowSeconds; }

	UFUNCTION(BlueprintPure, Category = "천복|우다다 콤보")
	float GetCurrentComboScoreMultiplier() const
	{
		return CurrentComboScoreMultiplier;
	}

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokComboChanged OnComboChanged;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokComboRewardTriggered OnComboRewardTriggered;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokGoldenItemRequested OnGoldenItemRequested;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokComboBroken OnComboBroken;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokWaveChanged OnWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokRemainingTimeChanged OnRemainingTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokAnnouncementChanged OnAnnouncementChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|점수")
	int32 CurrentScore = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보",
		meta = (DisplayName = "콤보 유지 시간", ClampMin = "0.1", Units = "s"))
	float ComboWindowSeconds = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보",
		meta = (DisplayName = "12콤보 황금 사료 확률", ClampMin = "0.0", ClampMax = "1.0"))
	float Combo12GoldenItemChance = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|우다다 콤보")
	int32 CurrentComboCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|우다다 콤보")
	int32 BestComboCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|우다다 콤보")
	float ComboTimeRemaining = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|우다다 콤보")
	float CurrentComboScoreMultiplier = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|웨이브")
	int32 CurrentWave = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|웨이브")
	int32 MaxWave = 3;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|웨이브")
	int32 RemainingTime = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|웨이브")
	int32 WaveDuration = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|게임 진행")
	FText AnnouncementText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "콤보 보상 사운드"))
	TObjectPtr<USoundBase> ComboRewardSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "콤보 끊김 사운드"))
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
