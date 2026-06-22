// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Ch03_MainMenuGameMode.generated.h"

class UCh03_MainMenuWidget;

UCLASS()
class CH03_TASK05_API ACh03_MainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACh03_MainMenuGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|UI")
	TSubclassOf<UCh03_MainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UCh03_MainMenuWidget> ActiveMainMenuWidget;
};
