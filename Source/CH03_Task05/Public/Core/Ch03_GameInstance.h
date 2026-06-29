// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Ch03_GameInstance.generated.h"

class UCh03_SaveGame;

UCLASS()
class CH03_TASK05_API UCh03_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCh03_GameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "천복|진행도")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "천복|진행도")
	void ResetProgress();

	UFUNCTION(BlueprintCallable, Category = "천복|진행도")
	void PrepareForLevel(FName LoadedLevelName);

	UFUNCTION(BlueprintCallable, Category = "천복|진행도")
	void CommitLevelScore(int32 NewTotalScore);

	UFUNCTION(BlueprintCallable, Category = "천복|저장")
	bool SubmitScore(int32 Score);

	UFUNCTION(BlueprintCallable, Category = "천복|진행도")
	bool PrepareTravelToLevel(FName LevelName);

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	int32 GetCommittedScore() const { return CommittedScore; }

	UFUNCTION(BlueprintPure, Category = "천복|저장")
	int32 GetHighestScore() const { return HighestScore; }

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	int32 GetCurrentLevelIndex() const { return CurrentLevelIndex; }

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	FName GetCurrentLevelName() const;

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	FName GetFirstLevelName() const;

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	FName GetNextLevelName() const;

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	bool HasNextLevel() const { return !GetNextLevelName().IsNone(); }

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	bool IsLevelAvailable(FName LevelName) const;

	UFUNCTION(BlueprintPure, Category = "천복|진행도")
	bool IsRunInProgress() const { return bRunInProgress; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|진행도", meta = (DisplayName = "레벨 진행 순서"))
	TArray<FName> LevelOrder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|저장", meta = (DisplayName = "저장 슬롯 이름"))
	FString SaveSlotName = TEXT("CheonbokLandSave");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|저장",
		meta = (DisplayName = "저장 사용자 인덱스", ClampMin = "0"))
	int32 SaveUserIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|진행도")
	int32 CommittedScore = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|진행도")
	int32 CurrentLevelIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|진행도")
	bool bRunInProgress = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "천복|저장")
	int32 HighestScore = 0;

	UPROPERTY(Transient)
	TObjectPtr<UCh03_SaveGame> SaveGameObject;

private:
	void LoadSaveData();
	bool SaveCurrentData() const;
};
