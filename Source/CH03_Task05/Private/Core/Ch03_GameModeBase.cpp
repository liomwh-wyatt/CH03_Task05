#include "Core/Ch03_GameModeBase.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/Widget.h"
#include "Core/Ch03_CheonbokController.h"
#include "Core/Ch03_GameInstance.h"
#include "Core/Ch03_GameStateBase.h"
#include "Engine/World.h"
#include "Environment/Ch03_WaveEnvironmentActor.h"
#include "GameFramework/Pawn.h"
#include "Items/Ch03_BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
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
	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> SlowingClass(
		TEXT("/Game/Blueprints/Items/BP_Item_Slowing"));
	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> ReverseControlClass(
		TEXT("/Game/Blueprints/Items/BP_Item_ReverseControl"));
	static ConstructorHelpers::FClassFinder<ACh03_BaseItem> GoldenFeedClass(
		TEXT("/Game/Blueprints/Items/BP_Item_GoldenFeed"));
	static ConstructorHelpers::FClassFinder<UCh03_GameResultWidget> ResultWidgetClass(
		TEXT("/Game/UI/WBP_GameResult"));

	if (ResultWidgetClass.Succeeded())
	{
		GameResultWidgetClass = ResultWidgetClass.Class;
	}

	if (GoldenFeedClass.Succeeded())
	{
		GoldenComboItemClass = GoldenFeedClass.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> WaveStartSoundFinder(
		TEXT("/Game/Audio/UI/S_WaveStart.S_WaveStart"));
	if (WaveStartSoundFinder.Succeeded())
	{
		WaveStartSound = WaveStartSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> GameOverSoundFinder(
		TEXT("/Game/Audio/UI/S_GameOver.S_GameOver"));
	if (GameOverSoundFinder.Succeeded())
	{
		GameOverSound = GameOverSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> LevelCompleteSoundFinder(
		TEXT("/Game/Audio/UI/S_GameClear.S_GameClear"));
	if (LevelCompleteSoundFinder.Succeeded())
	{
		LevelCompleteSound = LevelCompleteSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> GoldenAppearSoundFinder(
		TEXT("/Game/Audio/SFX/Items/S_GoldenWingSnack_Appear.S_GoldenWingSnack_Appear"));
	if (GoldenAppearSoundFinder.Succeeded())
	{
		GoldenItemAppearSound = GoldenAppearSoundFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> CommonMusicFinder(
		TEXT("/Game/Audio/BGM/BGM_Play_Common.BGM_Play_Common"));
	if (CommonMusicFinder.Succeeded())
	{
		CommonGameplayMusic = CommonMusicFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> LivingRoomMusicFinder(
		TEXT("/Game/Audio/BGM/BGM_LivingRoom.BGM_LivingRoom"));
	if (LivingRoomMusicFinder.Succeeded())
	{
		LivingRoomMusic = LivingRoomMusicFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> KitchenMusicFinder(
		TEXT("/Game/Audio/BGM/BGM_Kitchen.BGM_Kitchen"));
	if (KitchenMusicFinder.Succeeded())
	{
		KitchenMusic = KitchenMusicFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> CheonbokLandMusicFinder(
		TEXT("/Game/Audio/BGM/BGM_CheonbokLand.BGM_CheonbokLand"));
	if (CheonbokLandMusicFinder.Succeeded())
	{
		CheonbokLandMusic = CheonbokLandMusicFinder.Object;
	}

	AddSpawnEntry(WaveConfigs[0], SmallFeedClass.Class, 60.0f);
	AddSpawnEntry(WaveConfigs[0], LargeFeedClass.Class, 20.0f);
	AddSpawnEntry(WaveConfigs[0], HeartTreatClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[0], ToyBombClass.Class, 5.0f);

	AddSpawnEntry(WaveConfigs[1], SmallFeedClass.Class, 50.0f);
	AddSpawnEntry(WaveConfigs[1], LargeFeedClass.Class, 20.0f);
	AddSpawnEntry(WaveConfigs[1], HeartTreatClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[1], ToyBombClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[1], SlowingClass.Class, 8.0f);

	AddSpawnEntry(WaveConfigs[2], SmallFeedClass.Class, 40.0f);
	AddSpawnEntry(WaveConfigs[2], LargeFeedClass.Class, 20.0f);
	AddSpawnEntry(WaveConfigs[2], HeartTreatClass.Class, 15.0f);
	AddSpawnEntry(WaveConfigs[2], ToyBombClass.Class, 25.0f);
	AddSpawnEntry(WaveConfigs[2], SlowingClass.Class, 12.0f);
	AddSpawnEntry(WaveConfigs[2], ReverseControlClass.Class, 10.0f);
}

void ACh03_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyCurrentLevelFlowPreset();
	StartLevelMusic();

	CachedGameState = GetGameState<ACh03_GameStateBase>();
	CachedGameInstance = GetGameInstance<UCh03_GameInstance>();

	if (CachedGameState)
	{
		CachedGameState->OnGoldenItemRequested.AddUniqueDynamic(
			this,
			&ACh03_GameModeBase::HandleGoldenItemRequested);
	}

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
	CacheWaveEnvironmentActors();
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

	if (CachedGameState)
	{
		CachedGameState->OnGoldenItemRequested.RemoveDynamic(
			this,
			&ACh03_GameModeBase::HandleGoldenItemRequested);
	}

	BoundCharacter = nullptr;
	CachedGameState = nullptr;
	CachedGameInstance = nullptr;
	ActiveResultWidget = nullptr;
	StopLevelMusic();
	SpawnVolumes.Reset();
	WaveEnvironmentActors.Reset();
	bHasCachedWaveEnvironmentActors = false;

	Super::EndPlay(EndPlayReason);
}

void ACh03_GameModeBase::StartWaveLoop()
{
	ClearWaveTimers();
	StopAllSpawnVolumes(true);
	ApplyWaveEnvironmentState(0, WaveConfigs.Num());

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
	SetPlayerHUDVisible(true);

	CachedGameState->SetScore(
		CachedGameInstance
			? CachedGameInstance->GetCommittedScore()
			: 0);
	CachedGameState->ResetComboStats();
	CachedGameState->SetWave(0, WaveConfigs.Num());
	CachedGameState->SetWaveDuration(0);
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

void ACh03_GameModeBase::CacheWaveEnvironmentActors()
{
	WaveEnvironmentActors.Reset();
	bHasCachedWaveEnvironmentActors = true;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(
		this,
		ACh03_WaveEnvironmentActor::StaticClass(),
		FoundActors);

	for (AActor* FoundActor : FoundActors)
	{
		if (ACh03_WaveEnvironmentActor* EnvironmentActor =
			Cast<ACh03_WaveEnvironmentActor>(FoundActor))
		{
			WaveEnvironmentActors.Add(EnvironmentActor);
		}
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("GameMode found %d wave environment actor(s)."),
		WaveEnvironmentActors.Num());

	ApplyManagedEnvironmentRules();
}

void ACh03_GameModeBase::ApplyCurrentLevelFlowPreset()
{
	if (!bUseLevelFlowPresets || LevelFlowPresets.IsEmpty())
	{
		return;
	}

	const FName CurrentLevelName =
		FName(*UGameplayStatics::GetCurrentLevelName(this, true));

	for (const FCh03_LevelFlowPreset& LevelFlowPreset : LevelFlowPresets)
	{
		if (LevelFlowPreset.LevelName != CurrentLevelName)
		{
			continue;
		}

		if (!LevelFlowPreset.WaveConfigs.IsEmpty())
		{
			WaveConfigs = LevelFlowPreset.WaveConfigs;
		}

		if (!LevelFlowPreset.NextLevelName.IsNone())
		{
			NextLevelName = LevelFlowPreset.NextLevelName;
		}

		WaveEnvironmentRules = LevelFlowPreset.EnvironmentRules;

		UE_LOG(
			LogTemp,
			Log,
			TEXT("Applied level flow preset. Level=%s, Waves=%d, EnvironmentRules=%d"),
			*CurrentLevelName.ToString(),
			WaveConfigs.Num(),
			WaveEnvironmentRules.Num());
		return;
	}

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("No level flow preset found for %s. GameMode defaults are used."),
		*CurrentLevelName.ToString());
}

void ACh03_GameModeBase::ApplyManagedEnvironmentRules()
{
	if (WaveEnvironmentRules.IsEmpty() || WaveEnvironmentActors.IsEmpty())
	{
		return;
	}

	int32 TotalAppliedRuleCount = 0;

	for (const FCh03_WaveEnvironmentManagedRule& ManagedRule :
		WaveEnvironmentRules)
	{
		if (ManagedRule.ActorTag.IsNone())
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Wave environment managed rule skipped: ActorTag is empty."));
			continue;
		}

		int32 AppliedActorCount = 0;
		for (ACh03_WaveEnvironmentActor* EnvironmentActor :
			WaveEnvironmentActors)
		{
			if (!IsValid(EnvironmentActor)
				|| !EnvironmentActor->MatchesManagedRuleTag(
					ManagedRule.ActorTag))
			{
				continue;
			}

			EnvironmentActor->ApplyManagedRule(ManagedRule);
			++AppliedActorCount;
			++TotalAppliedRuleCount;
		}

		if (AppliedActorCount <= 0)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Wave environment managed rule found no actor. ActorTag=%s"),
				*ManagedRule.ActorTag.ToString());
		}
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Applied %d managed wave environment rule(s)."),
		TotalAppliedRuleCount);
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
		CachedGameState->SetWaveDuration(RemainingTime);
		CachedGameState->SetRemainingTime(RemainingTime);
	}

	const FText EnvironmentAnnouncement =
		ApplyWaveEnvironmentState(
			CurrentWaveIndex + 1,
			WaveConfigs.Num());

	ShowAnnouncement(
		FText::Format(
			NSLOCTEXT(
				"CheonbokGameFlow",
				"WaveStart",
				"Wave {0} Start!"),
			FText::AsNumber(CurrentWaveIndex + 1)),
		WaveStartAnnouncementDuration);
	PlayUISound(WaveStartSound);

	if (!EnvironmentAnnouncement.IsEmpty())
	{
		QueueAnnouncement(
			EnvironmentAnnouncement,
			WaveStartAnnouncementDuration + 0.05f,
			EnvironmentAnnouncementDuration);
	}

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
	ApplyWaveEnvironmentState(0, WaveConfigs.Num());

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
		CachedGameState->BreakComboWithReason(
			ECh03ComboBreakReason::GameFlow);
		CachedGameState->SetWaveDuration(0);
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
	ApplyWaveEnvironmentState(0, WaveConfigs.Num());

	if (CachedGameState)
	{
		CachedGameState->BreakComboWithReason(
			ECh03ComboBreakReason::GameFlow);
		CachedGameState->SetWaveDuration(0);
		CachedGameState->SetRemainingTime(0);
	}

	ShowAnnouncement(
		NSLOCTEXT(
			"CheonbokGameFlow",
			"GameOver",
			"Game Over"));
	PlayUISound(GameOverSound);

	UE_LOG(LogTemp, Error, TEXT("Game Over: Cheonbok is dead."));
	OnGameOver();
	ScheduleResultScreen(false);
}

void ACh03_GameModeBase::HandleGoldenItemRequested(
	const int32 ComboCount)
{
	if (CurrentPhase != ECh03_GamePhase::Playing || !GoldenComboItemClass)
	{
		return;
	}

	if (SpawnVolumes.IsEmpty())
	{
		CacheSpawnVolumes();
	}

	ACh03_BaseItem* SpawnedGoldenItem =
		TrySpawnGoldenComboItemFromVolumes();

	if (!IsValid(SpawnedGoldenItem) && bUseGoldenComboPlayerFallback)
	{
		SpawnedGoldenItem = SpawnGoldenComboItemNearPlayer();
	}

	if (!IsValid(SpawnedGoldenItem))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Failed to spawn golden combo item. Combo=%d"),
			ComboCount);
		OnGoldenComboItemSpawnFailed(ComboCount);
		return;
	}

	ShowAnnouncement(
		NSLOCTEXT(
			"CheonbokCombo",
			"GoldenWingSnackAppeared",
			"Golden wing snack appeared!"),
		1.6f);
	PlayUISound(GoldenItemAppearSound);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Golden combo item spawned. Combo=%d, Item=%s"),
		ComboCount,
		*GetNameSafe(SpawnedGoldenItem));

	OnGoldenComboItemSpawned(SpawnedGoldenItem, ComboCount);
}

ACh03_BaseItem* ACh03_GameModeBase::TrySpawnGoldenComboItemFromVolumes()
{
	if (!GoldenComboItemClass)
	{
		return nullptr;
	}

	if (SpawnVolumes.IsEmpty())
	{
		CacheSpawnVolumes();
	}

	TArray<ACh03_SpawnVolume*> ValidSpawnVolumes;
	for (ACh03_SpawnVolume* SpawnVolume : SpawnVolumes)
	{
		if (IsValid(SpawnVolume))
		{
			ValidSpawnVolumes.Add(SpawnVolume);
		}
	}

	if (ValidSpawnVolumes.IsEmpty())
	{
		return nullptr;
	}

	const int32 AttemptCount = FMath::Max(
		1,
		GoldenComboSpawnAttemptCount);

	for (int32 AttemptIndex = 0; AttemptIndex < AttemptCount; ++AttemptIndex)
	{
		ACh03_SpawnVolume* SelectedSpawnVolume =
			ValidSpawnVolumes[
				FMath::RandRange(0, ValidSpawnVolumes.Num() - 1)];

		if (!IsValid(SelectedSpawnVolume))
		{
			continue;
		}

		if (ACh03_BaseItem* SpawnedItem =
			SelectedSpawnVolume->SpawnItemOfClass(GoldenComboItemClass, true))
		{
			return SpawnedItem;
		}
	}

	return nullptr;
}

ACh03_BaseItem* ACh03_GameModeBase::SpawnGoldenComboItemNearPlayer() const
{
	if (!GetWorld() || !GoldenComboItemClass)
	{
		return nullptr;
	}

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return nullptr;
	}

	const FVector ForwardDirection =
		PlayerPawn->GetActorForwardVector().GetSafeNormal2D();
	const FVector SafeForwardDirection =
		ForwardDirection.IsNearlyZero()
			? FVector::ForwardVector
			: ForwardDirection;

	FVector SpawnLocation =
		PlayerPawn->GetActorLocation()
		+ SafeForwardDirection * GoldenComboFallbackDistanceFromPlayer;

	const FVector TraceStart = SpawnLocation + FVector::UpVector * 500.0f;
	const FVector TraceEnd = SpawnLocation - FVector::UpVector * 1500.0f;

	FHitResult GroundHit;
	FCollisionQueryParams TraceParams(
		SCENE_QUERY_STAT(Ch03GoldenComboFallbackGroundTrace),
		false,
		this);
	TraceParams.AddIgnoredActor(PlayerPawn);

	if (GetWorld()->LineTraceSingleByChannel(
		GroundHit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		TraceParams)
		&& GroundHit.bBlockingHit)
	{
		SpawnLocation.Z = GroundHit.ImpactPoint.Z + GoldenComboFallbackHeight;
	}
	else
	{
		SpawnLocation.Z += GoldenComboFallbackHeight;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = const_cast<ACh03_GameModeBase*>(this);
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<ACh03_BaseItem>(
		GoldenComboItemClass,
		SpawnLocation,
		FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f),
		SpawnParameters);
}

USoundBase* ACh03_GameModeBase::GetLevelMusic() const
{
	const FString CurrentLevelName =
		UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentLevelName == TEXT("L_LivingRoom") && LivingRoomMusic)
	{
		return LivingRoomMusic;
	}

	if (CurrentLevelName == TEXT("L_Kitchen") && KitchenMusic)
	{
		return KitchenMusic;
	}

	if (CurrentLevelName == TEXT("L_CheonbokLand") && CheonbokLandMusic)
	{
		return CheonbokLandMusic;
	}

	return CommonGameplayMusic;
}

void ACh03_GameModeBase::StartLevelMusic()
{
	if (LevelMusicComponent)
	{
		return;
	}

	USoundBase* Music = GetLevelMusic();
	if (!Music)
	{
		return;
	}

	LevelMusicComponent = UGameplayStatics::SpawnSound2D(
		this,
		Music,
		MusicVolumeMultiplier,
		1.0f,
		0.0f,
		nullptr,
		false,
		false);
}

void ACh03_GameModeBase::StopLevelMusic()
{
	if (LevelMusicComponent)
	{
		LevelMusicComponent->Stop();
		LevelMusicComponent = nullptr;
	}
}

void ACh03_GameModeBase::PlayUISound(USoundBase* Sound) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			Sound,
			UISoundVolumeMultiplier);
	}
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
	ApplyWaveEnvironmentState(0, WaveConfigs.Num());

	if (CachedGameState)
	{
		CachedGameState->BreakComboWithReason(
			ECh03ComboBreakReason::GameFlow);
		CachedGameState->SetWaveDuration(0);
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
	PlayUISound(LevelCompleteSound);

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

FText ACh03_GameModeBase::ApplyWaveEnvironmentState(
	const int32 CurrentWave,
	const int32 MaxWave)
{
	if (WaveEnvironmentActors.IsEmpty()
		&& !bHasCachedWaveEnvironmentActors)
	{
		CacheWaveEnvironmentActors();
	}

	TArray<FString> ActiveAnnouncementLines;
	TSet<FString> AddedAnnouncementLines;

	for (ACh03_WaveEnvironmentActor* EnvironmentActor :
		WaveEnvironmentActors)
	{
		if (!IsValid(EnvironmentActor))
		{
			continue;
		}

		const bool bNewlyActivated =
			EnvironmentActor->ApplyWaveState(CurrentWave, MaxWave);

		if (CurrentWave > 0
			&& bNewlyActivated
			&& !EnvironmentActor->GetActiveAnnouncementText().IsEmpty())
		{
			const FString AnnouncementLine =
				EnvironmentActor->GetActiveAnnouncementText().ToString();

			if (!AddedAnnouncementLines.Contains(AnnouncementLine))
			{
				AddedAnnouncementLines.Add(AnnouncementLine);
				ActiveAnnouncementLines.Add(AnnouncementLine);
			}
		}
	}

	if (ActiveAnnouncementLines.IsEmpty())
	{
		return FText::GetEmpty();
	}

	return FText::FromString(
		FString::Join(ActiveAnnouncementLines, TEXT("\n")));
}

void ACh03_GameModeBase::ShowAnnouncement(
	const FText& Message,
	const float DisplayDuration)
{
	if (!CachedGameState)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(QueuedAnnouncementTimerHandle);
	QueuedAnnouncementText = FText::GetEmpty();
	QueuedAnnouncementDuration = 0.0f;

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

void ACh03_GameModeBase::QueueAnnouncement(
	const FText& Message,
	const float Delay,
	const float DisplayDuration)
{
	if (Message.IsEmpty() || !GetWorld())
	{
		return;
	}

	QueuedAnnouncementText = Message;
	QueuedAnnouncementDuration = DisplayDuration;

	GetWorldTimerManager().ClearTimer(QueuedAnnouncementTimerHandle);

	if (Delay <= 0.0f)
	{
		ShowQueuedAnnouncement();
		return;
	}

	GetWorldTimerManager().SetTimer(
		QueuedAnnouncementTimerHandle,
		this,
		&ACh03_GameModeBase::ShowQueuedAnnouncement,
		Delay,
		false);
}

void ACh03_GameModeBase::ShowQueuedAnnouncement()
{
	const FText MessageToShow = QueuedAnnouncementText;
	const float DisplayDuration = QueuedAnnouncementDuration;

	QueuedAnnouncementText = FText::GetEmpty();
	QueuedAnnouncementDuration = 0.0f;

	ShowAnnouncement(MessageToShow, DisplayDuration);
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

	SetPlayerHUDVisible(false);

	const int32 FinalScore =
		CachedGameState ? CachedGameState->GetScore() : 0;
	const int32 BestComboCount =
		CachedGameState ? CachedGameState->GetBestComboCount() : 0;

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
		GetCurrentLevelDisplayName(),
		BestComboCount);

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

void ACh03_GameModeBase::SetPlayerHUDVisible(
	const bool bIsVisible)
{
	if (ACh03_CheonbokController* CheonbokController =
		Cast<ACh03_CheonbokController>(
			UGameplayStatics::GetPlayerController(this, 0)))
	{
		CheonbokController->SetGameHUDVisible(bIsVisible);
	}
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
	GetWorldTimerManager().ClearTimer(QueuedAnnouncementTimerHandle);
	QueuedAnnouncementText = FText::GetEmpty();
	QueuedAnnouncementDuration = 0.0f;
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
