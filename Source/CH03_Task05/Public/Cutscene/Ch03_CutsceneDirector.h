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

	UFUNCTION(BlueprintCallable, Category = "천복|컷신")
	void PlayCutscene(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "천복|컷신")
	void SkipCutscene();

	UFUNCTION(BlueprintPure, Category = "천복|컷신")
	bool IsCutscenePlaying() const { return bIsCutscenePlaying; }

	UFUNCTION(BlueprintPure, Category = "천복|컷신")
	FName GetCutsceneId() const { return CutsceneId; }

	UPROPERTY(BlueprintAssignable, Category = "천복|컷신")
	FOnCh03CutsceneFinished OnCutsceneFinished;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleSequenceFinished();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신", meta = (DisplayName = "컷신 식별자"))
	FName CutsceneId = TEXT("Cutscene");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신", meta = (DisplayName = "레벨 시퀀스"))
	TObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신", meta = (DisplayName = "시작 시 자동 재생"))
	bool bAutoPlayOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신", meta = (DisplayName = "플레이어 입력 비활성화"))
	bool bDisablePlayerInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신", meta = (DisplayName = "게임 화면 숨김"))
	bool bHideGameHUD = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신", meta = (DisplayName = "컷신 후 마우스 커서 복원"))
	bool bRestoreMouseCursorAfterCutscene = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|컷신",
		meta = (DisplayName = "예비 지속시간", ClampMin = "0.0", Units = "s"))
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
