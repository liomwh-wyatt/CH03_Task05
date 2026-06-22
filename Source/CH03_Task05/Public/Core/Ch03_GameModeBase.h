#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Spawning/Ch03_SpawnVolume.h"
#include "Ch03_GameModeBase.generated.h"

class ACh03_CheonbokCharacter;
class ACh03_GameStateBase;
class UCh03_GameResultWidget;
class UCh03_GameInstance;

UENUM(BlueprintType)
enum class ECh03_GamePhase : uint8
{
	Waiting		UMETA(DisplayName = "Waiting"),
	Playing		UMETA(DisplayName = "Playing"),
	WaveInterval	UMETA(DisplayName = "Wave Interval"),
	GameOver	UMETA(DisplayName = "Game Over"),
	LevelComplete	UMETA(DisplayName = "Level Complete")
};

USTRUCT(BlueprintType)
struct FCh03_WaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave",
		meta = (ClampMin = "1", Units = "s"))
	int32 Duration = 45;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave",
		meta = (ClampMin = "0"))
	int32 InitialItemsPerVolume = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave",
		meta = (ClampMin = "1"))
	int32 MaxAliveItemsPerVolume = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave",
		meta = (ClampMin = "0.1", Units = "s"))
	float SpawnInterval = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FCh03_SpawnItemEntry> SpawnEntries;
};

UCLASS()
class CH03_TASK05_API ACh03_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACh03_GameModeBase();

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Game Flow")
	ECh03_GamePhase GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Game Flow")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Game Flow")
	void StartWaveLoop();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	TArray<FCh03_WaveConfig> WaveConfigs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave",
		meta = (ClampMin = "0.0", Units = "s"))
	float FirstWaveDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave",
		meta = (ClampMin = "0.0", Units = "s"))
	float WaveIntervalDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave")
	bool bAutoStartWaveLoop = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Presentation",
		meta = (ClampMin = "0.0", Units = "s"))
	float WaveStartAnnouncementDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result")
	TSubclassOf<UCh03_GameResultWidget> GameResultWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result",
		meta = (ClampMin = "0.0", Units = "s"))
	float ResultScreenDelay = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result")
	FName NextLevelName = NAME_None;

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Game Flow")
	void OnWaveStarted(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Game Flow")
	void OnGameOver();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Game Flow")
	void OnLevelCompleted();

private:
	void CacheSpawnVolumes();
	void BindCharacterEvents();
	void StartCurrentWave();
	void TickWaveTimer();
	void EndCurrentWave();
	void CompleteLevel();
	void StopAllSpawnVolumes(bool bClearItems);
	void ShowAnnouncement(const FText& Message, float DisplayDuration = 0.0f);
	void ClearAnnouncement();
	void ScheduleResultScreen(bool bWasVictory);
	void ShowPendingResultScreen();
	void LockPlayerInput();
	FText GetCurrentLevelDisplayName() const;
	void ClearWaveTimers();
	void ClearGameTimers();
	void SetGamePhase(ECh03_GamePhase NewPhase);

	UFUNCTION()
	void HandleCharacterDeath();

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACh03_SpawnVolume>> SpawnVolumes;

	UPROPERTY(Transient)
	TObjectPtr<ACh03_CheonbokCharacter> BoundCharacter;

	UPROPERTY(Transient)
	TObjectPtr<ACh03_GameStateBase> CachedGameState;

	UPROPERTY(Transient)
	TObjectPtr<UCh03_GameInstance> CachedGameInstance;

	UPROPERTY(Transient)
	TObjectPtr<UCh03_GameResultWidget> ActiveResultWidget;

	ECh03_GamePhase CurrentPhase = ECh03_GamePhase::Waiting;
	int32 CurrentWaveIndex = INDEX_NONE;
	int32 RemainingTime = 0;
	bool bPendingResultWasVictory = false;

	FTimerHandle WaveTimerHandle;
	FTimerHandle WaveTransitionTimerHandle;
	FTimerHandle CharacterBindRetryTimerHandle;
	FTimerHandle AnnouncementTimerHandle;
	FTimerHandle ResultScreenTimerHandle;
};
