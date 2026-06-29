// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Ch03_SaveGame.generated.h"

UCLASS()
class CH03_TASK05_API UCh03_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|저장", meta = (DisplayName = "최고 점수"))
	int32 HighestScore = 0;
};
