// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Ch03_GameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCheonbokScoreChanged,
	int32, NewScore);

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

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Score")
	int32 GetScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Score")
	void AddScore(int32 Amount);

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

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave")
	void SetWave(int32 NewCurrentWave, int32 NewMaxWave);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave")
	void SetRemainingTime(int32 NewRemainingTime);

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Game Flow")
	FText GetAnnouncementText() const { return AnnouncementText; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Game Flow")
	void SetAnnouncementText(const FText& NewAnnouncement);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Game Flow")
	void ClearAnnouncementText();

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokWaveChanged OnWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokRemainingTimeChanged OnRemainingTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokAnnouncementChanged OnAnnouncementChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Score")
	int32 CurrentScore = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 CurrentWave = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 MaxWave = 3;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	int32 RemainingTime = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Game Flow")
	FText AnnouncementText;
};
