// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_GameHUDWidget.generated.h"

class ACh03_GameStateBase;
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

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD")
	void OnScoreUpdated(int32 NewScore);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

private:
	void BindToGameState();

	UPROPERTY()
	TObjectPtr<ACh03_GameStateBase> BoundGameState;
};
