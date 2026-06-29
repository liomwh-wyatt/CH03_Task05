// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Ch03_MainMenuGameMode.generated.h"

class UCh03_MainMenuWidget;
class UAudioComponent;
class USoundBase;

UCLASS()
class CH03_TASK05_API ACh03_MainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACh03_MainMenuGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "메인 메뉴 위젯 클래스"))
	TSubclassOf<UCh03_MainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악", meta = (DisplayName = "메인 메뉴 배경음악"))
	TObjectPtr<USoundBase> MainMenuMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악",
		meta = (DisplayName = "배경음악 볼륨 배율", ClampMin = "0.0"))
	float MusicVolumeMultiplier = 0.48f;

	UPROPERTY(Transient)
	TObjectPtr<UCh03_MainMenuWidget> ActiveMainMenuWidget;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> MainMenuMusicComponent;

private:
	void StartMainMenuMusic();
	void StopMainMenuMusic();
};
