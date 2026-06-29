// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Feedback/Ch03_FeedbackCue.h"
#include "GameFramework/Actor.h"
#include "Ch03_JumpPadActor.generated.h"

class ACh03_CheonbokCharacter;
class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCh03JumpPadActivated,
	ACh03_CheonbokCharacter*, CheonbokCharacter);

UCLASS()
class CH03_TASK05_API ACh03_JumpPadActor : public AActor
{
	GENERATED_BODY()

public:
	ACh03_JumpPadActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "천복|점프 발판",
		meta = (DisplayName = "점프 발판 활성화 설정"))
	void SetJumpPadEnabled(bool bNewEnabled);

	UFUNCTION(BlueprintPure, Category = "천복|점프 발판",
		meta = (DisplayName = "점프 발판 활성 상태"))
	bool IsJumpPadEnabled() const { return bIsJumpPadEnabled; }

	UFUNCTION(BlueprintCallable, Category = "천복|점프 발판",
		meta = (DisplayName = "점프 발판 발동"))
	bool ActivateJumpPad(ACh03_CheonbokCharacter* CheonbokCharacter);

	UPROPERTY(BlueprintAssignable, Category = "천복|점프 발판|이벤트",
		meta = (DisplayName = "점프 발판 발동 이벤트"))
	FOnCh03JumpPadActivated OnJumpPadActivated;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleLaunchVolumeBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|컴포넌트",
		meta = (DisplayName = "씬 루트"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|컴포넌트",
		meta = (DisplayName = "발판 메쉬"))
	TObjectPtr<UStaticMeshComponent> PadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|컴포넌트",
		meta = (DisplayName = "발동 판정 볼륨"))
	TObjectPtr<UBoxComponent> LaunchVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|상태",
		meta = (DisplayName = "시작 시 활성화"))
	bool bStartEnabled = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|상태",
		meta = (DisplayName = "현재 활성화 상태"))
	bool bIsJumpPadEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|발사",
		meta = (DisplayName = "위로 튀어오르는 힘", ClampMin = "0.0", Units = "cm/s",
			ToolTip = "천복이가 발판에 닿았을 때 위쪽으로 받는 속도입니다. 값이 클수록 높게 튀어오릅니다."))
	float LaunchStrength = 1050.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|발사",
		meta = (DisplayName = "앞 방향 보너스 힘", ClampMin = "0.0", Units = "cm/s",
			ToolTip = "발판의 앞 방향으로 추가할 속도입니다. 0이면 위로만 튀어오릅니다."))
	float ForwardBoostStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|발사",
		meta = (DisplayName = "수평 속도 덮어쓰기",
			ToolTip = "켜면 기존 좌우 이동 속도를 지우고 발판이 지정한 수평 속도로 바꿉니다. 끄면 기존 이동 관성을 유지합니다."))
	bool bOverrideHorizontalVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|발사",
		meta = (DisplayName = "수직 속도 덮어쓰기",
			ToolTip = "켜면 떨어지는 중이어도 지정한 위쪽 속도로 바꿉니다. 일반적인 점프 발판은 켜두는 편이 안정적입니다."))
	bool bOverrideVerticalVelocity = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|발사",
		meta = (DisplayName = "재발동 대기시간", ClampMin = "0.0", Units = "s",
			ToolTip = "같은 발판이 다시 발동하기까지 기다리는 시간입니다. 너무 낮으면 겹침 판정이 연속으로 들어올 수 있습니다."))
	float ReactivationDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|점프 발판|피드백",
		meta = (DisplayName = "발동 피드백",
			ToolTip = "점프 발판이 천복이를 튀어오르게 할 때 재생할 사운드와 이펙트입니다."))
	FCh03FeedbackCue ActivationFeedback;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|띠용 연출",
		meta = (DisplayName = "띠용 스케일 연출 사용",
			ToolTip = "켜면 점프 발판이 발동할 때 발판 메쉬가 짧게 눌렸다가 늘어나며 원래 크기로 돌아옵니다."))
	bool bUseBounceScaleAnimation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|띠용 연출",
		meta = (DisplayName = "띠용 연출 시간", EditCondition = "bUseBounceScaleAnimation", ClampMin = "0.01", Units = "s",
			ToolTip = "발판 메쉬가 눌림, 늘어남, 복귀를 끝내는 데 걸리는 시간입니다."))
	float BounceAnimationDuration = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|띠용 연출",
		meta = (DisplayName = "눌림 스케일 배율", EditCondition = "bUseBounceScaleAnimation",
			ToolTip = "발동 직후 발판이 납작하게 눌리는 스케일 배율입니다. X/Y는 커지고 Z는 작아지면 탄성이 잘 보입니다."))
	FVector SquashScaleMultiplier = FVector(1.16f, 1.16f, 0.62f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|점프 발판|띠용 연출",
		meta = (DisplayName = "늘어남 스케일 배율", EditCondition = "bUseBounceScaleAnimation",
			ToolTip = "눌린 뒤 튀어오르는 느낌을 주는 스케일 배율입니다. X/Y는 작아지고 Z는 커지면 탄성이 잘 보입니다."))
	FVector StretchScaleMultiplier = FVector(0.9f, 0.9f, 1.28f);

private:
	bool CanActivateJumpPad() const;
	void RefreshLaunchVolumeCollision() const;
	void StartBounceScaleAnimation();
	void UpdateBounceScaleAnimation(float DeltaSeconds);
	FVector GetBounceScaleAtNormalizedTime(float NormalizedTime) const;

	float LastActivationTime = -BIG_NUMBER;
	float BounceAnimationElapsed = 0.0f;
	bool bIsBounceAnimationActive = false;
	FVector RestingPadMeshRelativeScale = FVector::OneVector;
};
