// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Ch03_GameInstance.generated.h"

UCLASS()
class CH03_TASK05_API UCh03_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCh03_GameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Progress")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Progress")
	void PrepareForLevel(FName LoadedLevelName);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Progress")
	void CommitLevelScore(int32 NewTotalScore);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Progress")
	bool PrepareTravelToLevel(FName LevelName);

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	int32 GetCommittedScore() const { return CommittedScore; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	int32 GetCurrentLevelIndex() const { return CurrentLevelIndex; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	FName GetCurrentLevelName() const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	FName GetFirstLevelName() const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	FName GetNextLevelName() const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	bool HasNextLevel() const { return !GetNextLevelName().IsNone(); }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	bool IsLevelAvailable(FName LevelName) const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Progress")
	bool IsRunInProgress() const { return bRunInProgress; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Progress")
	TArray<FName> LevelOrder;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Progress")
	int32 CommittedScore = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Progress")
	int32 CurrentLevelIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Progress")
	bool bRunInProgress = false;
};
