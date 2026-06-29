#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Ch03_CheonbokController.generated.h"

class UInputAction;
class UInputMappingContext;
class ACh03_CutsceneDirector;
class UCh03_GameHUDWidget;
class UCh03_PauseMenuWidget;

UCLASS()
class CH03_TASK05_API ACh03_CheonbokController : public APlayerController
{
	GENERATED_BODY()

public:
	ACh03_CheonbokController();

	bool IsSprintInputHeld() const;

	UFUNCTION(BlueprintCallable, Category = "천복|일시정지")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "천복|일시정지")
	void OpenPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "천복|일시정지")
	void ClosePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "천복|일시정지")
	void ReturnToMainMenuFromPause();

	UFUNCTION(BlueprintCallable, Category = "천복|컷신")
	void RegisterActiveCutsceneDirector(ACh03_CutsceneDirector* CutsceneDirector);

	UFUNCTION(BlueprintCallable, Category = "천복|컷신")
	void ClearActiveCutsceneDirector(ACh03_CutsceneDirector* CutsceneDirector);

	UFUNCTION(BlueprintCallable, Category = "천복|컷신")
	bool SkipActiveCutscene();

	UFUNCTION(BlueprintCallable, Category = "천복|화면")
	void SetGameHUDVisible(bool bIsVisible);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|입력", meta = (DisplayName = "기본 입력 매핑 컨텍스트"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|입력", meta = (DisplayName = "이동 입력 액션"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|입력", meta = (DisplayName = "시점 입력 액션"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|입력", meta = (DisplayName = "점프 입력 액션"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|입력", meta = (DisplayName = "달리기 입력 액션"))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "게임 화면 위젯 클래스"))
	TSubclassOf<UCh03_GameHUDWidget> GameHUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "천복|화면")
	TObjectPtr<UCh03_GameHUDWidget> GameHUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "일시정지 메뉴 위젯 클래스"))
	TSubclassOf<UCh03_PauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "천복|화면")
	TObjectPtr<UCh03_PauseMenuWidget> PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "메인 메뉴 레벨 이름"))
	FName MainMenuLevelName = TEXT("L_MainMenu");

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void AddDefaultMappingContext();
	void CreateGameHUD();
	void ApplyGameHUDVisibility();
	void ApplyGameplayInputMode();
	void ApplyPauseInputMode();
	void HandleSprintStarted();
	void HandleSprintStopped();

	UPROPERTY(Transient)
	TObjectPtr<ACh03_CutsceneDirector> ActiveCutsceneDirector;

	bool bSprintInputHeld = false;
	bool bShouldShowGameHUD = true;
};
