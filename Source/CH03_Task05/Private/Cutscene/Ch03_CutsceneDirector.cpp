#include "Cutscene/Ch03_CutsceneDirector.h"

#include "Core/Ch03_CheonbokController.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"

ACh03_CutsceneDirector::ACh03_CutsceneDirector()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACh03_CutsceneDirector::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoPlayOnBeginPlay)
	{
		PlayCutscene(GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr);
	}
}

void ACh03_CutsceneDirector::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			FallbackFinishTimerHandle);
	}

	if (bIsCutscenePlaying)
	{
		FinishCutscene();
	}

	Super::EndPlay(EndPlayReason);
}

void ACh03_CutsceneDirector::PlayCutscene(
	APlayerController* PlayerController)
{
	if (bIsCutscenePlaying)
	{
		return;
	}

	CachedPlayerController = PlayerController
		? PlayerController
		: (GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr);

	bIsCutscenePlaying = true;
	bIsFinishingCutscene = false;
	bPreviousShowMouseCursor = CachedPlayerController
		? CachedPlayerController->bShowMouseCursor
		: false;

	ApplyCutsceneInputMode();

	if (ACh03_CheonbokController* CheonbokController =
		Cast<ACh03_CheonbokController>(CachedPlayerController))
	{
		CheonbokController->RegisterActiveCutsceneDirector(this);
	}

	if (LevelSequence)
	{
		FMovieSceneSequencePlaybackSettings PlaybackSettings;
		PlaybackSettings.bAutoPlay = false;

		ALevelSequenceActor* CreatedSequenceActor = nullptr;
		ActiveSequencePlayer =
			ULevelSequencePlayer::CreateLevelSequencePlayer(
				GetWorld(),
				LevelSequence,
				PlaybackSettings,
				CreatedSequenceActor);
		ActiveSequenceActor = CreatedSequenceActor;

		if (ActiveSequencePlayer)
		{
			ActiveSequencePlayer->OnFinished.AddDynamic(
				this,
				&ACh03_CutsceneDirector::HandleSequenceFinished);
			ActiveSequencePlayer->Play();
			return;
		}
	}

	if (FallbackDuration > 0.0f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			FallbackFinishTimerHandle,
			this,
			&ACh03_CutsceneDirector::FinishCutscene,
			FallbackDuration,
			false);
		return;
	}

	FinishCutscene();
}

void ACh03_CutsceneDirector::SkipCutscene()
{
	if (!bIsCutscenePlaying)
	{
		return;
	}

	if (ActiveSequencePlayer)
	{
		ActiveSequencePlayer->Stop();
	}

	FinishCutscene();
}

void ACh03_CutsceneDirector::HandleSequenceFinished()
{
	FinishCutscene();
}

void ACh03_CutsceneDirector::FinishCutscene()
{
	if (!bIsCutscenePlaying || bIsFinishingCutscene)
	{
		return;
	}

	bIsFinishingCutscene = true;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			FallbackFinishTimerHandle);
	}

	if (ActiveSequencePlayer)
	{
		ActiveSequencePlayer->OnFinished.RemoveDynamic(
			this,
			&ACh03_CutsceneDirector::HandleSequenceFinished);
		ActiveSequencePlayer = nullptr;
	}

	ActiveSequenceActor = nullptr;

	RestoreCutsceneInputMode();

	if (ACh03_CheonbokController* CheonbokController =
		Cast<ACh03_CheonbokController>(CachedPlayerController))
	{
		CheonbokController->ClearActiveCutsceneDirector(this);
	}

	bIsCutscenePlaying = false;
	bIsFinishingCutscene = false;
	OnCutsceneFinished.Broadcast(CutsceneId);
}

void ACh03_CutsceneDirector::ApplyCutsceneInputMode()
{
	if (!CachedPlayerController)
	{
		return;
	}

	if (bDisablePlayerInput)
	{
		CachedPlayerController->SetIgnoreMoveInput(true);
		CachedPlayerController->SetIgnoreLookInput(true);
	}

	CachedPlayerController->bShowMouseCursor = false;
	CachedPlayerController->SetInputMode(FInputModeGameOnly());

	if (bHideGameHUD)
	{
		if (ACh03_CheonbokController* CheonbokController =
			Cast<ACh03_CheonbokController>(CachedPlayerController))
		{
			CheonbokController->SetGameHUDVisible(false);
		}
	}
}

void ACh03_CutsceneDirector::RestoreCutsceneInputMode()
{
	if (!CachedPlayerController)
	{
		return;
	}

	if (bDisablePlayerInput)
	{
		CachedPlayerController->SetIgnoreMoveInput(false);
		CachedPlayerController->SetIgnoreLookInput(false);
	}

	CachedPlayerController->bShowMouseCursor =
		bRestoreMouseCursorAfterCutscene || bPreviousShowMouseCursor;
	CachedPlayerController->SetInputMode(FInputModeGameOnly());

	if (bHideGameHUD)
	{
		if (ACh03_CheonbokController* CheonbokController =
			Cast<ACh03_CheonbokController>(CachedPlayerController))
		{
			CheonbokController->SetGameHUDVisible(true);
		}
	}
}
