#pragma once

#include "CoreMinimal.h"
#include "Environment/Ch03_WaveEnvironmentActor.h"
#include "GameFramework/GameModeBase.h"
#include "Spawning/Ch03_SpawnVolume.h"
#include "Ch03_GameModeBase.generated.h"

class ACh03_CheonbokCharacter;
class ACh03_GameStateBase;
class UCh03_GameResultWidget;
class UCh03_GameInstance;
class UAudioComponent;
class USoundBase;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "웨이브",
		meta = (DisplayName = "웨이브 시간", ClampMin = "1", Units = "s"))
	int32 Duration = 45;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "웨이브",
		meta = (DisplayName = "볼륨별 초기 아이템 수", ClampMin = "0"))
	int32 InitialItemsPerVolume = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "웨이브",
		meta = (DisplayName = "볼륨별 최대 활성 아이템 수", ClampMin = "1"))
	int32 MaxAliveItemsPerVolume = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "웨이브",
		meta = (DisplayName = "스폰 간격", ClampMin = "0.1", Units = "s"))
	float SpawnInterval = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "웨이브", meta = (DisplayName = "스폰 아이템 목록"))
	TArray<FCh03_SpawnItemEntry> SpawnEntries;
};

USTRUCT(BlueprintType)
struct FCh03_LevelFlowPreset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "레벨 흐름",
		meta = (DisplayName = "레벨 이름", ToolTip = "적용할 레벨 이름입니다. 예: L_LivingRoom, L_Kitchen, L_CheonbokLand"))
	FName LevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "레벨 흐름",
		meta = (DisplayName = "다음 레벨 이름", ToolTip = "이 레벨을 클리어한 뒤 이동할 다음 레벨입니다. 비워두면 기존 설정을 유지합니다."))
	FName NextLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "레벨 흐름",
		meta = (DisplayName = "웨이브 구성", ToolTip = "이 레벨에서 사용할 웨이브 구성입니다. 비워두면 게임모드의 기본 웨이브 구성을 유지합니다."))
	TArray<FCh03_WaveConfig> WaveConfigs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "레벨 흐름",
		meta = (DisplayName = "방해물 규칙", ToolTip = "이 레벨에서 태그로 찾아 적용할 이동식/회전식 방해물 규칙입니다."))
	TArray<FCh03_WaveEnvironmentManagedRule> EnvironmentRules;
};

UCLASS()
class CH03_TASK05_API ACh03_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACh03_GameModeBase();

	UFUNCTION(BlueprintPure, Category = "천복|게임 진행")
	ECh03_GamePhase GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "천복|게임 진행")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	UFUNCTION(BlueprintCallable, Category = "천복|게임 진행")
	void StartWaveLoop();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|레벨 흐름",
		meta = (DisplayName = "레벨 흐름 프리셋 사용", ToolTip = "켜면 현재 레벨 이름과 같은 프리셋을 찾아 웨이브와 방해물 규칙을 한 번에 적용합니다."))
	bool bUseLevelFlowPresets = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|레벨 흐름",
		meta = (DisplayName = "레벨 흐름 프리셋", ToolTip = "레벨별 웨이브 구성과 방해물 규칙을 모아 관리하는 목록입니다."))
	TArray<FCh03_LevelFlowPreset> LevelFlowPresets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|웨이브", meta = (DisplayName = "웨이브 구성"))
	TArray<FCh03_WaveConfig> WaveConfigs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|웨이브",
		meta = (DisplayName = "첫 웨이브 시작 지연", ClampMin = "0.0", Units = "s"))
	float FirstWaveDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|웨이브",
		meta = (DisplayName = "웨이브 간격 시간", ClampMin = "0.0", Units = "s"))
	float WaveIntervalDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|웨이브", meta = (DisplayName = "웨이브 자동 시작"))
	bool bAutoStartWaveLoop = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|웨이브 방해물|일괄 관리",
		meta = (DisplayName = "웨이브 방해물 규칙", ToolTip = "레벨 프리셋을 쓰지 않을 때 적용할 방해물 일괄 관리 규칙입니다. Actor Tags가 같은 방해물에 적용됩니다."))
	TArray<FCh03_WaveEnvironmentManagedRule> WaveEnvironmentRules;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|연출",
		meta = (DisplayName = "웨이브 시작 문구 시간", ClampMin = "0.0", Units = "s"))
	float WaveStartAnnouncementDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|연출",
		meta = (DisplayName = "방해물 경고 문구 시간", ClampMin = "0.0", Units = "s"))
	float EnvironmentAnnouncementDuration = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|결과", meta = (DisplayName = "결과 화면 위젯 클래스"))
	TSubclassOf<UCh03_GameResultWidget> GameResultWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|결과",
		meta = (DisplayName = "결과 화면 표시 지연", ClampMin = "0.0", Units = "s"))
	float ResultScreenDelay = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|결과", meta = (DisplayName = "다음 레벨 이름"))
	FName NextLevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보", meta = (DisplayName = "황금 사료 아이템 클래스"))
	TSubclassOf<ACh03_BaseItem> GoldenComboItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보",
		meta = (DisplayName = "황금 사료 스폰 시도 횟수", ClampMin = "1"))
	int32 GoldenComboSpawnAttemptCount = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보", meta = (DisplayName = "황금 사료 예비 스폰 사용"))
	bool bUseGoldenComboPlayerFallback = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보",
		meta = (DisplayName = "예비 스폰 플레이어 거리", ClampMin = "0.0", Units = "cm"))
	float GoldenComboFallbackDistanceFromPlayer = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|우다다 콤보",
		meta = (DisplayName = "예비 스폰 높이", ClampMin = "0.0", Units = "cm"))
	float GoldenComboFallbackHeight = 140.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "웨이브 시작 사운드"))
	TObjectPtr<USoundBase> WaveStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "게임 오버 사운드"))
	TObjectPtr<USoundBase> GameOverSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "레벨 클리어 사운드"))
	TObjectPtr<USoundBase> LevelCompleteSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드", meta = (DisplayName = "황금 사료 등장 사운드"))
	TObjectPtr<USoundBase> GoldenItemAppearSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악", meta = (DisplayName = "공통 게임 배경음악"))
	TObjectPtr<USoundBase> CommonGameplayMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악", meta = (DisplayName = "거실 배경음악"))
	TObjectPtr<USoundBase> LivingRoomMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악", meta = (DisplayName = "주방 배경음악"))
	TObjectPtr<USoundBase> KitchenMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악", meta = (DisplayName = "천복랜드 배경음악"))
	TObjectPtr<USoundBase> CheonbokLandMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드|배경음악",
		meta = (DisplayName = "배경음악 볼륨 배율", ClampMin = "0.0"))
	float MusicVolumeMultiplier = 0.48f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|사운드",
		meta = (DisplayName = "화면 사운드 볼륨 배율", ClampMin = "0.0"))
	float UISoundVolumeMultiplier = 0.9f;

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|게임 진행")
	void OnWaveStarted(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|게임 진행")
	void OnGameOver();

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|게임 진행")
	void OnLevelCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|우다다 콤보|피드백")
	void OnGoldenComboItemSpawned(
		ACh03_BaseItem* SpawnedItem,
		int32 ComboCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|우다다 콤보|피드백")
	void OnGoldenComboItemSpawnFailed(int32 ComboCount);

private:
	void ApplyCurrentLevelFlowPreset();
	void CacheSpawnVolumes();
	void CacheWaveEnvironmentActors();
	void ApplyManagedEnvironmentRules();
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
	USoundBase* GetLevelMusic() const;
	void StartLevelMusic();
	void StopLevelMusic();
	void PlayUISound(USoundBase* Sound) const;

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

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> LevelMusicComponent;

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
