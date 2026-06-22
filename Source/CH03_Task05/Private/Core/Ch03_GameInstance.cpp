// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Core/Ch03_GameInstance.h"

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

	CommittedScore = 0;
	CurrentLevelIndex = 0;
	bRunInProgress = false;
}

void UCh03_GameInstance::StartNewGame()
{
	CommittedScore = 0;
	CurrentLevelIndex = 0;
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
