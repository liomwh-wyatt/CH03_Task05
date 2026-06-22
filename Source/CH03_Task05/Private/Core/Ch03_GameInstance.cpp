// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Core/Ch03_GameInstance.h"

#include "Core/Ch03_SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"

UCh03_GameInstance::UCh03_GameInstance()
{
	LevelOrder =
	{
		TEXT("L_LivingRoom"),
		TEXT("L_Kitchen"),
		TEXT("L_CheonbokLand")
	};
}

void UCh03_GameInstance::Init()
{
	Super::Init();

	LoadSaveData();
	ResetProgress();
}

void UCh03_GameInstance::ResetProgress()
{
	CommittedScore = 0;
	CurrentLevelIndex = 0;
	bRunInProgress = false;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok Land progress reset."));
}

void UCh03_GameInstance::StartNewGame()
{
	ResetProgress();
	bRunInProgress = true;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("New Cheonbok Land run started."));
}

void UCh03_GameInstance::PrepareForLevel(
	const FName LoadedLevelName)
{
	if (!bRunInProgress)
	{
		StartNewGame();
	}

	const int32 LoadedLevelIndex =
		LevelOrder.IndexOfByKey(LoadedLevelName);

	if (LoadedLevelIndex != INDEX_NONE)
	{
		CurrentLevelIndex = LoadedLevelIndex;
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Level %s is not registered in GameInstance LevelOrder."),
			*LoadedLevelName.ToString());
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Progress prepared for %s. LevelIndex=%d, CommittedScore=%d"),
		*LoadedLevelName.ToString(),
		CurrentLevelIndex,
		CommittedScore);
}

void UCh03_GameInstance::CommitLevelScore(
	const int32 NewTotalScore)
{
	CommittedScore = FMath::Max(0, NewTotalScore);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Level score committed: %d"),
		CommittedScore);
}

bool UCh03_GameInstance::SubmitScore(
	const int32 Score)
{
	const int32 SanitizedScore = FMath::Max(0, Score);

	if (SanitizedScore <= HighestScore)
	{
		return false;
	}

	HighestScore = SanitizedScore;

	if (!SaveGameObject)
	{
		SaveGameObject = Cast<UCh03_SaveGame>(
			UGameplayStatics::CreateSaveGameObject(
				UCh03_SaveGame::StaticClass()));
	}

	if (SaveGameObject)
	{
		SaveGameObject->HighestScore = HighestScore;
	}

	const bool bSaved = SaveCurrentData();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("New Cheonbok Land high score submitted: %d. Saved=%s"),
		HighestScore,
		bSaved ? TEXT("true") : TEXT("false"));

	return true;
}

bool UCh03_GameInstance::PrepareTravelToLevel(
	const FName LevelName)
{
	if (!IsLevelAvailable(LevelName))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Cannot travel to unavailable level: %s"),
			*LevelName.ToString());
		return false;
	}

	const int32 DestinationIndex =
		LevelOrder.IndexOfByKey(LevelName);

	if (DestinationIndex != INDEX_NONE)
	{
		CurrentLevelIndex = DestinationIndex;
	}

	bRunInProgress = true;
	return true;
}

FName UCh03_GameInstance::GetCurrentLevelName() const
{
	return LevelOrder.IsValidIndex(CurrentLevelIndex)
		? LevelOrder[CurrentLevelIndex]
		: NAME_None;
}

FName UCh03_GameInstance::GetFirstLevelName() const
{
	return LevelOrder.IsEmpty()
		? NAME_None
		: LevelOrder[0];
}

FName UCh03_GameInstance::GetNextLevelName() const
{
	const int32 NextLevelIndex = CurrentLevelIndex + 1;

	if (!LevelOrder.IsValidIndex(NextLevelIndex))
	{
		return NAME_None;
	}

	const FName CandidateLevelName =
		LevelOrder[NextLevelIndex];

	return IsLevelAvailable(CandidateLevelName)
		? CandidateLevelName
		: NAME_None;
}

bool UCh03_GameInstance::IsLevelAvailable(
	const FName LevelName) const
{
	if (LevelName.IsNone())
	{
		return false;
	}

	FString PackageName = LevelName.ToString();
	if (!PackageName.StartsWith(TEXT("/")))
	{
		PackageName = FString::Printf(
			TEXT("/Game/Maps/%s"),
			*PackageName);
	}

	return FPackageName::DoesPackageExist(PackageName);
}

void UCh03_GameInstance::LoadSaveData()
{
	USaveGame* LoadedSaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(
		SaveSlotName,
		SaveUserIndex))
	{
		LoadedSaveGame = UGameplayStatics::LoadGameFromSlot(
			SaveSlotName,
			SaveUserIndex);
	}

	SaveGameObject = Cast<UCh03_SaveGame>(LoadedSaveGame);

	if (!SaveGameObject)
	{
		SaveGameObject = Cast<UCh03_SaveGame>(
			UGameplayStatics::CreateSaveGameObject(
				UCh03_SaveGame::StaticClass()));
	}

	HighestScore = SaveGameObject
		? FMath::Max(0, SaveGameObject->HighestScore)
		: 0;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok Land save loaded. HighestScore=%d"),
		HighestScore);
}

bool UCh03_GameInstance::SaveCurrentData() const
{
	if (!SaveGameObject)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Cannot save Cheonbok Land data: SaveGameObject is missing."));
		return false;
	}

	return UGameplayStatics::SaveGameToSlot(
		SaveGameObject,
		SaveSlotName,
		SaveUserIndex);
}
