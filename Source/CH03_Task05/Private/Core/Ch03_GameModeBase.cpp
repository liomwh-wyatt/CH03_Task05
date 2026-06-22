#include "Core/Ch03_GameModeBase.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/Widget.h"
#include "Core/Ch03_CheonbokController.h"
#include "Core/Ch03_GameInstance.h"
#include "Core/Ch03_GameStateBase.h"
#include "Engine/World.h"
#include "Items/Ch03_BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UI/Ch03_GameResultWidget.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
void AddSpawnEntry(
	FCh03_WaveConfig& WaveConfig,
	const TSubclassOf<ACh03_BaseItem> ItemClass,
	const float Weight)
{
	if (!ItemClass || Weight <= 0.0f)
	{
		return;
	}

	FCh03_SpawnItemEntry& NewEntry =
		WaveConfig.SpawnEntries.AddDefaulted_GetRef();
	NewEntry.ItemClass = ItemClass;
	NewEntry.Weight = Weight;
}
}

ACh03_GameModeBase::ACh03_GameModeBase()
{
	PlayerControllerClass = ACh03_CheonbokController::StaticClass();
	DefaultPawnClass = ACh03_CheonbokCharacter::StaticClass();
	GameStateClass = ACh03_GameStateBase::StaticClass();

	WaveConfigs.SetNum(3);

	WaveConfigs[0].Duration = 45;
	WaveConfigs[0].InitialItemsPerVolume = 2;
	WaveConfigs[0].MaxAliveItemsPerVolume = 3;
	WaveConfigs[0].SpawnInterval = 1.8f;

	WaveConfigs[1].Duration = 40;
	WaveConfigs[1].InitialItemsPerVolume = 3;
	WaveConfigs[1].MaxAliveItemsPerVolume = 4;
	WaveConfigs[1].SpawnInterval = 1.4f;

	WaveConfigs[2].Duration = 35;
	WaveConfigs[2].InitialItemsPerVolume = 4;
	WaveConfigs[2].MaxAliveItemsPerVolume = 5;
	WaveConfigs[2].SpawnInterval = 1.0f;

	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> SmallFeedClass(
		TEXT("/Game/Blueprints/Items/BP_Item_SmallFeed"));
	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> LargeFeedClass(
		TEXT("/Game/Blueprints/Items/BP_Item_Largefeed"));
	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> HeartTreatClass(
		TEXT("/Game/Blueprints/Items/BP_Item_HeartTreat"));
	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> ToyBombClass(
		TEXT("/Game/Blueprints/Items/BP_Item_ToyBomb"));
	static ConstructorHelpers::FClassFinder<UCh03_GameResultWidget> ResultWidgetClass(
		TEXT("/Game/UI/WBP_GameResult"));

	if (ResultWidgetClass.Succeeded())
	{
		GameResultWidgetClass = ResultWidgetClass.Class;
	}

	AddSpawnEntry(WaveConfigs[0], SmallFeedClass.Class, 60.0f);
	AddSpawnEntry(WaveConfigs[0], LargeFeedClass.Class, 20.0f);
	AddSpawnEntry(WaveConfigs[0], HeartTreatClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[0], ToyBombClass.Class, 5.0f);

	AddSpawnEntry(WaveConfigs[1], SmallFeedClass.Class, 50.0f);
	AddSpawnEntry(WaveConfigs[1], LargeFeedClass.Class, 20.0f);
	AddSpawnEntry(WaveConfigs[1], HeartTreatClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[1], ToyBombClass.Class, 15.0f);

	AddSpawnEntry(WaveConfigs[2], SmallFeedClass.Class, 40.0f);
	AddSpawnEntry(WaveConfigs[2], LargeFeedClass.Class, 20.0f);
	AddSpawnEntry(WaveConfigs[2], HeartTreatClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[2], ToyBombClass.Class, 25.0f);
}

void ACh03_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CachedGameState = GetGameState<ACh03_GameStateBase>();
	CachedGameInstance = GetGameInstance<UCh03_GameInstance>();

	if (CachedGameInstance)
	{
		CachedGameInstance->PrepareForLevel(
			FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Ch03_GameInstance is not configured."));
	}

	CacheSpawnVolumes();
	BindCharacterEvents();

	if (bAutoStartWaveLoop)
	{
		StartWaveLoop();
	}
}

void ACh03_GameModeBase::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	ClearGameTimers();

	if (BoundCharacter)
	{
		BoundCharacter->OnCharacterDeath.RemoveDynamic(
			this,
			&ACh03_GameModeBase::HandleCharacterDeath);
	}

	BoundCharacter = nullptr;
	CachedGameState = nullptr;
	CachedGameInstance = nullptr;
	ActiveResultWidget = nullptr;
	SpawnVolumes.Reset();

	Super::EndPlay(EndPlayReason);
}

void ACh03_GameModeBase::StartWaveLoop()
{
	ClearWaveTimers();
	StopAllSpawnVolumes(true);

	if (WaveConfigs.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GameMode cannot start: WaveConfigs is empty."));
		return;
	}

	if (SpawnVolumes.IsEmpty())
	{
		CacheSpawnVolumes();
	}

	if (SpawnVolumes.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GameMode cannot start: no Ch03_SpawnVolume found."));
		return;
	}

	if (!CachedGameState)
	{
		CachedGameState = GetGameState<ACh03_GameStateBase>();
	}

	if (!CachedGameState)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GameMode cannot start: Ch03_GameStateBase is missing."));
		return;
	}

	CurrentWaveIndex = 0;
	RemainingTime = 0;
	SetGamePhase(ECh03_GamePhase::Waiting);

	CachedGameState->SetScore(
		CachedGameInstance
			? CachedGameInstance->GetCommittedScore()
			: 0);
	CachedGameState->SetWave(0, WaveConfigs.Num());
	CachedGameState->SetRemainingTime(0);
	ShowAnnouncement(
		NSLOCTEXT(
			"CheonbokGameFlow",
			"GetReady",
			"Get Ready!"));

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Wave loop will start in %.1f seconds."),
		FirstWaveDelay);

	if (FirstWaveDelay <= 0.0f)
	{
		StartCurrentWave();
		return;
	}

	GetWorldTimerManager().SetTimer(
		WaveTransitionTimerHandle,
		this,
		&ACh03_GameModeBase::StartCurrentWave,
		FirstWaveDelay,
		false);
}

void ACh03_GameModeBase::CacheSpawnVolumes()
{
	SpawnVolumes.Reset();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(
		this,
		ACh03_SpawnVolume::StaticClass(),
		FoundActors);

	for (AActor* FoundActor : FoundActors)
	{
		if (ACh03_SpawnVolume* SpawnVolume =
			Cast<ACh03_SpawnVolume>(FoundActor))
		{
			SpawnVolumes.Add(SpawnVolume);
		}
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("GameMode found %d spawn volume(s)."),
		SpawnVolumes.Num());
}

void ACh03_GameModeBase::BindCharacterEvents()
{
	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(
			UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!CheonbokCharacter)
	{
		if (GetWorld()
			&& !GetWorldTimerManager().IsTimerActive(
				CharacterBindRetryTimerHandle))
		{
			GetWorldTimerManager().SetTimer(
				CharacterBindRetryTimerHandle,
				this,
				&ACh03_GameModeBase::BindCharacterEvents,
				0.2f,
				true);
		}
		return;
	}

	GetWorldTimerManager().ClearTimer(CharacterBindRetryTimerHandle);

	if (BoundCharacter && BoundCharacter != CheonbokCharacter)
	{
		BoundCharacter->OnCharacterDeath.RemoveDynamic(
			this,
			&ACh03_GameModeBase::HandleCharacterDeath);
	}

	BoundCharacter = CheonbokCharacter;
	BoundCharacter->OnCharacterDeath.AddUniqueDynamic(
		this,
		&ACh03_GameModeBase::HandleCharacterDeath);
}

void ACh03_GameModeBase::StartCurrentWave()
{
	if (CurrentPhase == ECh03_GamePhase::GameOver
		|| CurrentPhase == ECh03_GamePhase::LevelComplete)
	{
		return;
	}

	if (!WaveConfigs.IsValidIndex(CurrentWaveIndex))
	{
		CompleteLevel();
		return;
	}

	const FCh03_WaveConfig& WaveConfig =
		WaveConfigs[CurrentWaveIndex];

	float TotalSpawnWeight = 0.0f;
	int32 ValidSpawnEntryCount = 0;
	for (const FCh03_SpawnItemEntry& Entry : WaveConfig.SpawnEntries)
	{
		if (Entry.ItemClass && Entry.Weight > 0.0f)
		{
			TotalSpawnWeight += Entry.Weight;
			++ValidSpawnEntryCount;
		}
	}

	if (ValidSpawnEntryCount == 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Wave %d has no item entries. Spawn volumes will keep their existing item entries."),
			CurrentWaveIndex + 1);
	}

	SetGamePhase(ECh03_GamePhase::Playing);
	RemainingTime = FMath::Max(1, WaveConfig.Duration);

	if (CachedGameState)
	{
		CachedGameState->SetWave(
			CurrentWaveIndex + 1,
			WaveConfigs.Num());
		CachedGameState->SetRemainingTime(RemainingTime);
	}

	ShowAnnouncement(
		FText::Format(
			NSLOCTEXT(
				"CheonbokGameFlow",
				"WaveStart",
				"Wave {0} Start!"),
			FText::AsNumber(CurrentWaveIndex + 1)),
		WaveStartAnnouncementDuration);

	for (ACh03_SpawnVolume* SpawnVolume : SpawnVolumes)
	{
		if (!IsValid(SpawnVolume))
		{
			continue;
		}

		SpawnVolume->ApplyWaveSettings(
			WaveConfig.SpawnEntries,
			WaveConfig.InitialItemsPerVolume,
			WaveConfig.MaxAliveItemsPerVolume,
			WaveConfig.SpawnInterval);
		SpawnVolume->SpawnInitialItems();
		SpawnVolume->StartSpawning();
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ACh03_GameModeBase::TickWaveTimer,
		1.0f,
		true);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Wave %d / %d started. Duration=%d, Entries=%d, TotalWeight=%.1f"),
		CurrentWaveIndex + 1,
		WaveConfigs.Num(),
		RemainingTime,
		ValidSpawnEntryCount,
		TotalSpawnWeight);

	OnWaveStarted(CurrentWaveIndex + 1, WaveConfigs.Num());
}

void ACh03_GameModeBase::TickWaveTimer()
{
	if (CurrentPhase != ECh03_GamePhase::Playing)
	{
		return;
	}

	RemainingTime = FMath::Max(0, RemainingTime - 1);

	if (CachedGameState)
	{
		CachedGameState->SetRemainingTime(RemainingTime);
	}

	if (RemainingTime <= 0)
	{
		EndCurrentWave();
	}
}

void ACh03_GameModeBase::EndCurrentWave()
{
	if (CurrentPhase != ECh03_GamePhase::Playing)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	StopAllSpawnVolumes(true);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Wave %d / %d ended."),
		CurrentWaveIndex + 1,
		WaveConfigs.Num());

	ShowAnnouncement(
		FText::Format(
			NSLOCTEXT(
				"CheonbokGameFlow",
				"WaveComplete",
				"Wave {0} Complete!"),
			FText::AsNumber(CurrentWaveIndex + 1)));

	if (CurrentWaveIndex >= WaveConfigs.Num() - 1)
	{
		CompleteLevel();
		return;
	}

	++CurrentWaveIndex;
	RemainingTime = 0;
	SetGamePhase(ECh03_GamePhase::WaveInterval);

	if (CachedGameState)
	{
		CachedGameState->SetRemainingTime(0);
	}

	if (WaveIntervalDuration <= 0.0f)
	{
		StartCurrentWave();
		return;
	}

	GetWorldTimerManager().SetTimer(
		WaveTransitionTimerHandle,
		this,
		&ACh03_GameModeBase::StartCurrentWave,
		WaveIntervalDuration,
		false);
}

void ACh03_GameModeBase::HandleCharacterDeath()
{
	if (CurrentPhase == ECh03_GamePhase::GameOver
		|| CurrentPhase == ECh03_GamePhase::LevelComplete)
	{
		return;
	}

	SetGamePhase(ECh03_GamePhase::GameOver);
	ClearGameTimers();
	StopAllSpawnVolumes(true);

	if (CachedGameState)
	{
		CachedGameState->SetRemainingTime(0);
	}

	ShowAnnouncement(
		NSLOCTEXT(
			"CheonbokGameFlow",
			"GameOver",
			"Game Over"));

	UE_LOG(LogTemp, Error, TEXT("Game Over: Cheonbok is dead."));
	OnGameOver();
	ScheduleResultScreen(false);
}

void ACh03_GameModeBase::CompleteLevel()
{
	if (CurrentPhase == ECh03_GamePhase::LevelComplete)
	{
		return;
	}

	SetGamePhase(ECh03_GamePhase::LevelComplete);
	ClearGameTimers();
	StopAllSpawnVolumes(true);

	if (CachedGameState)
	{
		CachedGameState->SetRemainingTime(0);
	}

	const FText LevelDisplayName =
		GetCurrentLevelDisplayName();

	ShowAnnouncement(
		FText::Format(
			NSLOCTEXT(
				"CheonbokGameFlow",
				"LevelComplete",
				"{0} Complete!"),
			LevelDisplayName));

	const int32 FinalScore =
		CachedGameState ? CachedGameState->GetScore() : 0;

	if (CachedGameInstance)
	{
		CachedGameInstance->CommitLevelScore(FinalScore);
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s completed. Final score=%d"),
		*LevelDisplayName.ToString(),
		FinalScore);

	OnLevelCompleted();
	ScheduleResultScreen(true);
}

void ACh03_GameModeBase::StopAllSpawnVolumes(
	const bool bClearItems)
{
	for (ACh03_SpawnVolume* SpawnVolume : SpawnVolumes)
	{
		if (!IsValid(SpawnVolume))
		{
			continue;
		}

		SpawnVolume->StopSpawning();

		if (bClearItems)
		{
			SpawnVolume->ClearSpawnedItems();
		}
	}
}

void ACh03_GameModeBase::ShowAnnouncement(
	const FText& Message,
	const float DisplayDuration)
{
	if (!CachedGameState)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(AnnouncementTimerHandle);
	CachedGameState->SetAnnouncementText(Message);

	if (DisplayDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			AnnouncementTimerHandle,
			this,
			&ACh03_GameModeBase::ClearAnnouncement,
			DisplayDuration,
			false);
	}
}

void ACh03_GameModeBase::ClearAnnouncement()
{
	if (CachedGameState)
	{
		CachedGameState->ClearAnnouncementText();
	}
}

void ACh03_GameModeBase::ScheduleResultScreen(
	const bool bWasVictory)
{
	bPendingResultWasVictory = bWasVictory;
	LockPlayerInput();

	if (ResultScreenDelay <= 0.0f)
	{
		ShowPendingResultScreen();
		return;
	}

	GetWorldTimerManager().SetTimer(
		ResultScreenTimerHandle,
		this,
		&ACh03_GameModeBase::ShowPendingResultScreen,
		ResultScreenDelay,
		false);
}

void ACh03_GameModeBase::ShowPendingResultScreen()
{
	APlayerController* PlayerController =
		UGameplayStatics::GetPlayerController(this, 0);

	if (!PlayerController)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Cannot show result screen: player controller is missing."));
		return;
	}

	if (!GameResultWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Cannot show result screen: GameResultWidgetClass is not set."));
		return;
	}

	if (ActiveResultWidget)
	{
		ActiveResultWidget->RemoveFromParent();
		ActiveResultWidget = nullptr;
	}

	ActiveResultWidget = CreateWidget<UCh03_GameResultWidget>(
		PlayerController,
		GameResultWidgetClass);

	if (!ActiveResultWidget)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to create Ch03_GameResultWidget."));
		return;
	}

	const int32 FinalScore =
		CachedGameState ? CachedGameState->GetScore() : 0;

	ActiveResultWidget->AddToViewport(100);
	FName ResolvedNextLevelName = NextLevelName;

	if (CachedGameInstance)
	{
		if (ResolvedNextLevelName.IsNone())
		{
			ResolvedNextLevelName =
				CachedGameInstance->GetNextLevelName();
		}
		else if (!CachedGameInstance->IsLevelAvailable(
			ResolvedNextLevelName))
		{
			ResolvedNextLevelName = NAME_None;
		}
	}

	ActiveResultWidget->InitializeResult(
		bPendingResultWasVictory,
		FinalScore,
		ResolvedNextLevelName,
		GetCurrentLevelDisplayName());

	FInputModeUIOnly InputMode;
	if (UWidget* InitialFocusWidget =
		ActiveResultWidget->GetInitialFocusWidget())
	{
		InputMode.SetWidgetToFocus(
			InitialFocusWidget->TakeWidget());
	}
	InputMode.SetLockMouseToViewportBehavior(
		EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	UGameplayStatics::SetGamePaused(this, true);
}

void ACh03_GameModeBase::LockPlayerInput()
{
	if (APlayerController* PlayerController =
		UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
	}
}

FText ACh03_GameModeBase::GetCurrentLevelDisplayName() const
{
	const FString CurrentLevelName =
		UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentLevelName == TEXT("L_LivingRoom"))
	{
		return NSLOCTEXT(
			"CheonbokLevel",
			"LivingRoom",
			"Living Room");
	}

	if (CurrentLevelName == TEXT("L_Kitchen"))
	{
		return NSLOCTEXT(
			"CheonbokLevel",
			"Kitchen",
			"Kitchen");
	}

	if (CurrentLevelName == TEXT("L_CheonbokLand"))
	{
		return NSLOCTEXT(
			"CheonbokLevel",
			"CheonbokLand",
			"Cheonbok Land");
	}

	FString FallbackDisplayName = CurrentLevelName;
	FallbackDisplayName.RemoveFromStart(TEXT("L_"));
	FallbackDisplayName.ReplaceInline(TEXT("_"), TEXT(" "));

	return FText::FromString(FallbackDisplayName);
}

void ACh03_GameModeBase::ClearGameTimers()
{
	ClearWaveTimers();

	if (!GetWorld())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(CharacterBindRetryTimerHandle);
	GetWorldTimerManager().ClearTimer(ResultScreenTimerHandle);
}

void ACh03_GameModeBase::ClearWaveTimers()
{
	if (!GetWorld())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTransitionTimerHandle);
	GetWorldTimerManager().ClearTimer(AnnouncementTimerHandle);
}

void ACh03_GameModeBase::SetGamePhase(
	const ECh03_GamePhase NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		return;
	}

	CurrentPhase = NewPhase;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Game phase changed: %s"),
		*UEnum::GetValueAsString(CurrentPhase));
}
