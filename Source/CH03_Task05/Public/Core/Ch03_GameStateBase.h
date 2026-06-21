// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Ch03_GameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCheonbokScoreChanged,
	int32, NewScore);

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

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokScoreChanged OnScoreChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Score")
	int32 CurrentScore = 0;
};
