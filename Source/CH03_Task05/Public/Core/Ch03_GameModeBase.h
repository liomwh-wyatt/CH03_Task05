#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Spawning/Ch03_SpawnVolume.h"
#include "Ch03_GameModeBase.generated.h"

class ACh03_CheonbokCharacter;
class ACh03_WaveEnvironmentActor;
class ACh03_GameStateBase;
class UCh03_GameResultWidget;
class UCh03_GameInstance;

UENUM(BlueprintType)
enum class ECh03_GamePhase : uint8
{
	Waiting		UMETA(DisplayName = "대기"),
	Playing		UMETA(DisplayName = "진행 중"),
	WaveInterval	UMETA(DisplayName = "웨이브 간격"),
	GameOver	UMETA(DisplayName = "게임 오버"),
	LevelComplete	UMETA(DisplayName = "레벨 클리어")
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Presentation",
		meta = (ClampMin = "0.0", Units = "s"))
	float EnvironmentAnnouncementDuration = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result")
	TSubclassOf<UCh03_GameResultWidget> GameResultWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result",
		meta = (ClampMin = "0.0", Units = "s"))
	float ResultScreenDelay = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Result")
	FName NextLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo")
	TSubclassOf<ACh03_BaseItem> GoldenComboItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo",
		meta = (ClampMin = "1"))
	int32 GoldenComboSpawnAttemptCount = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo")
	bool bUseGoldenComboPlayerFallback = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo",
		meta = (ClampMin = "0.0", Units = "cm"))
	float GoldenComboFallbackDistanceFromPlayer = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Combo",
		meta = (ClampMin = "0.0", Units = "cm"))
	float GoldenComboFallbackHeight = 140.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Game Flow")
	void OnWaveStarted(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Game Flow")
	void OnGameOver();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Game Flow")
	void OnLevelCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Combo|Feedback")
	void OnGoldenComboItemSpawned(
		ACh03_BaseItem* SpawnedItem,
		int32 ComboCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Combo|Feedback")
	void OnGoldenComboItemSpawnFailed(int32 ComboCount);

private:
	void CacheSpawnVolumes();
	void CacheWaveEnvironmentActors();
	void BindCharacterEvents();
	void StartCurrentWave();
	void TickWaveTimer();
	void EndCurrentWave();
	void CompleteLevel();
	void StopAllSpawnVolumes(bool bClearItems);
	FText ApplyWaveEnvironmentState(int32 CurrentWave, int32 MaxWave);
	void ShowAnnouncement(const FText& Message, float DisplayDuration = 0.0f);
	void QueueAnnouncement(
		const FText& Message,
		float Delay,
		float DisplayDuration);
	void ShowQueuedAnnouncement();
	void ClearAnnouncement();
	void ScheduleResultScreen(bool bWasVictory);
	void ShowPendingResultScreen();
	void SetPlayerHUDVisible(bool bIsVisible);
	void LockPlayerInput();
	FText GetCurrentLevelDisplayName() const;
	void ClearWaveTimers();
	void ClearGameTimers();
	void SetGamePhase(ECh03_GamePhase NewPhase);
	ACh03_BaseItem* TrySpawnGoldenComboItemFromVolumes();
	ACh03_BaseItem* SpawnGoldenComboItemNearPlayer() const;

	UFUNCTION()
	void HandleCharacterDeath();

	UFUNCTION()
	void HandleGoldenItemRequested(int32 ComboCount);

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACh03_SpawnVolume>> SpawnVolumes;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACh03_WaveEnvironmentActor>> WaveEnvironmentActors;

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
	bool bHasCachedWaveEnvironmentActors = false;
	float QueuedAnnouncementDuration = 0.0f;
	FText QueuedAnnouncementText;

	FTimerHandle WaveTimerHandle;
	FTimerHandle WaveTransitionTimerHandle;
	FTimerHandle CharacterBindRetryTimerHandle;
	FTimerHandle AnnouncementTimerHandle;
	FTimerHandle QueuedAnnouncementTimerHandle;
	FTimerHandle ResultScreenTimerHandle;
};
