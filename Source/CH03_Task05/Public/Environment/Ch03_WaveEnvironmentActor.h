#pragma once

#include "CoreMinimal.h"
#include "Feedback/Ch03_FeedbackCue.h"
#include "GameFramework/Actor.h"
#include "Ch03_WaveEnvironmentActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ACh03_CheonbokCharacter;

UENUM(BlueprintType)
enum class ECh03WaveEnvironmentPathEndBehavior : uint8
{
	Reverse UMETA(DisplayName = "끝에서 되돌아가기"),
	Loop UMETA(DisplayName = "처음으로 이어서 이동"),
	Stop UMETA(DisplayName = "끝에서 정지")
};

UCLASS()
class CH03_TASK05_API ACh03_WaveEnvironmentActor : public AActor
{
	GENERATED_BODY()

public:
	ACh03_WaveEnvironmentActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave Environment")
	bool ApplyWaveState(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave Environment")
	void SetEnvironmentActive(bool bNewActive);

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave Environment")
	bool IsEnvironmentActive() const { return bIsEnvironmentActive; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave Environment")
	bool IsActiveForWave(int32 CurrentWave) const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave Environment")
	FText GetActiveAnnouncementText() const { return ActiveAnnouncementText; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleHazardBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Wave Environment")
	void OnEnvironmentActivated(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Wave Environment")
	void OnEnvironmentDeactivated(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|Wave Environment|Feedback")
	void OnHazardHitFeedback(
		ACh03_CheonbokCharacter* CheonbokCharacter,
		float AppliedDamage);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	TObjectPtr<UBoxComponent> HazardVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (ClampMin = "1"))
	int32 ActivateFromWave = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (ClampMin = "0", ToolTip = "0이면 마지막 웨이브까지 유지합니다."))
	int32 DeactivateAfterWave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	bool bHideWhenInactive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	bool bAffectPlayerOnOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bAffectPlayerOnOverlap", ClampMin = "0.0"))
	float DamageAmount = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bAffectPlayerOnOverlap"))
	bool bApplySlowOnOverlap = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bApplySlowOnOverlap", ClampMin = "0.1", Units = "s"))
	float SlowDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bApplySlowOnOverlap", ClampMin = "0.1", ClampMax = "1.0"))
	float SlowMultiplier = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bAffectPlayerOnOverlap"))
	bool bApplyKnockbackOnOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bApplyKnockbackOnOverlap", ClampMin = "0.0"))
	float KnockbackStrength = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bApplyKnockbackOnOverlap", ClampMin = "0.0"))
	float KnockbackUpwardStrength = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bApplyKnockbackOnOverlap"))
	bool bUseMovementDirectionForKnockback = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment",
		meta = (EditCondition = "bApplyKnockbackOnOverlap", ClampMin = "0.0", Units = "s"))
	float KnockbackCooldown = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	FText ActiveAnnouncementText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = "Cheonbok|Wave Environment|Feedback")
	FCh03FeedbackCue ActivationFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = "Cheonbok|Wave Environment|Feedback")
	FCh03FeedbackCue DeactivationFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = "Cheonbok|Wave Environment|Feedback")
	FCh03FeedbackCue HazardHitFeedback;

private:
	FVector GetKnockbackDirection(
		const ACh03_CheonbokCharacter* CheonbokCharacter) const;
	void CacheInitialLocationIfNeeded();
	void UpdateActiveMovement(float DeltaSeconds);
	void UpdatePathMovement(float DeltaSeconds);
	void UpdateFacingRotation(float DeltaSeconds);
	int32 GetPathPointCount() const;
	int32 GetPathSegmentCount() const;
	float GetPathTotalLength() const;
	float GetPathSegmentLength(int32 SegmentIndex) const;
	FVector GetPathPointWorldLocation(int32 PointIndex) const;
	FVector GetPathSegmentStartLocation(int32 SegmentIndex) const;
	FVector GetPathSegmentEndLocation(int32 SegmentIndex) const;
	FVector GetPathLocationAtDistance(float DistanceAlongPath) const;
	bool HasValidPathPointActors() const;
	int32 GetValidPathPointActorCount() const;
	FVector GetValidPathPointActorLocation(int32 ValidPathPointIndex) const;
	void ResetMovement();

	int32 LastAppliedWave = 0;
	int32 LastAppliedMaxWave = 0;
	bool bIsEnvironmentActive = false;

	FVector InitialActorLocation = FVector::ZeroVector;
	FRotator InitialActorRotation = FRotator::ZeroRotator;
	float LastKnockbackTime = -BIG_NUMBER;
	FVector CurrentMovementDirection = FVector::ZeroVector;
	float PathTravelDistance = 0.0f;
	float PathTravelDirection = 1.0f;
	bool bHasCachedInitialLocation = false;
	bool bHasReachedPathEnd = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement")
	bool bMoveWhenActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive", ClampMin = "0.0", Units = "cm/s",
			ToolTip = "경로를 따라 이동하는 속도입니다."))
	float MovementSpeed = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "켜면 배치된 위치를 첫 번째 경로 지점으로 사용합니다."))
	bool bUseInitialLocationAsFirstPathPoint = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "레벨에 배치한 액터를 경로 지점으로 사용합니다. 하나라도 지정하면 상대 좌표 목록은 사용하지 않습니다."))
	TArray<TObjectPtr<AActor>> PathPointActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "배치 위치 기준 상대 좌표로 경로를 지정합니다. 경로 액터가 없을 때만 사용합니다."))
	TArray<FVector> PathPointOffsets = { FVector(400.0f, 0.0f, 0.0f) };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "마지막 경로 지점에 도착한 뒤의 처리 방식입니다."))
	ECh03WaveEnvironmentPathEndBehavior PathEndBehavior =
		ECh03WaveEnvironmentPathEndBehavior::Reverse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Facing",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "켜면 이동 방향을 바라보며 회전합니다."))
	bool bFaceMovementDirection = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Facing",
		meta = (EditCondition = "bMoveWhenActive && bFaceMovementDirection", ClampMin = "0.0",
			ToolTip = "회전 보간 속도입니다. 값이 클수록 빠르게 돌고, 0이면 즉시 회전합니다."))
	float FacingRotationInterpSpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Facing",
		meta = (EditCondition = "bMoveWhenActive && bFaceMovementDirection",
			ToolTip = "모델의 앞 방향이 언리얼 기준 앞 방향(+X)과 다를 때 보정합니다. 보통 요 회전만 조정합니다."))
	FRotator FacingRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "켜면 비활성화될 때 배치 위치와 초기 회전으로 되돌립니다."))
	bool bResetLocationWhenInactive = true;
};
