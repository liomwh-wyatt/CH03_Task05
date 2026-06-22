#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ch03_CutsceneDirector.generated.h"

class ALevelSequenceActor;
class ULevelSequence;
class ULevelSequencePlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCh03CutsceneFinished,
	FName,
	CutsceneId);

UCLASS()
class CH03_TASK05_API ACh03_CutsceneDirector : public AActor
{
	GENERATED_BODY()

public:
	ACh03_CutsceneDirector();

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Cutscene")
	void PlayCutscene(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Cutscene")
	void SkipCutscene();

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Cutscene")
	bool IsCutscenePlaying() const { return bIsCutscenePlaying; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Cutscene")
	FName GetCutsceneId() const { return CutsceneId; }

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Cutscene")
	FOnCh03CutsceneFinished OnCutsceneFinished;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleSequenceFinished();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene")
	FName CutsceneId = TEXT("Cutscene");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene")
	TObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene")
	bool bAutoPlayOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene")
	bool bDisablePlayerInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene")
	bool bHideGameHUD = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene")
	bool bRestoreMouseCursorAfterCutscene = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Cutscene",
		meta = (ClampMin = "0.0", Units = "s"))
	float FallbackDuration = 0.0f;

private:
	void FinishCutscene();
	void ApplyCutsceneInputMode();
	void RestoreCutsceneInputMode();

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> ActiveSequencePlayer;

	UPROPERTY(Transient)
	TObjectPtr<ALevelSequenceActor> ActiveSequenceActor;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> CachedPlayerController;

	FTimerHandle FallbackFinishTimerHandle;
	bool bIsCutscenePlaying = false;
	bool bIsFinishingCutscene = false;
	bool bPreviousShowMouseCursor = false;
};
