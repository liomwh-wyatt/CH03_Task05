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

UENUM(BlueprintType)
enum class ECh03WaveEnvironmentRotationAxis : uint8
{
	X UMETA(DisplayName = "X축"),
	Y UMETA(DisplayName = "Y축"),
	Z UMETA(DisplayName = "Z축")
};

USTRUCT(BlueprintType)
struct FCh03_WaveEnvironmentManagedRule
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed",
		meta = (ToolTip = "이 규칙을 적용할 방해물 태그입니다. 레벨에 배치한 방해물 Actor Tags에 같은 이름을 넣습니다."))
	FName ActorTag = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed",
		meta = (ClampMin = "1", ToolTip = "몇 번째 웨이브부터 방해물을 활성화할지 정합니다."))
	int32 ActivateFromWave = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed",
		meta = (ClampMin = "0", ToolTip = "0이면 마지막 웨이브까지 유지합니다."))
	int32 DeactivateAfterWave = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed",
		meta = (ToolTip = "비활성화 중 방해물을 숨길지 정합니다."))
	bool bHideWhenInactive = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed",
		meta = (ToolTip = "활성화될 때 HUD에 보여줄 문구입니다. 비워두면 방해물 BP의 문구를 유지합니다."))
	FText ActiveAnnouncementText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (ToolTip = "켜면 피해, 넉백, 디버프 값을 이 규칙으로 덮어씁니다."))
	bool bOverrideHazard = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard", ToolTip = "천복이가 닿았을 때 피해나 디버프를 줄지 정합니다. 장식용 회전목마라면 끕니다."))
	bool bAffectPlayerOnOverlap = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap", ClampMin = "0.05", Units = "s",
			ToolTip = "천복이가 계속 닿아 있을 때 피해와 디버프를 다시 적용하는 간격입니다."))
	float ContactEffectInterval = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap", ClampMin = "0.0",
			ToolTip = "닿았을 때 줄 피해량입니다. 0이면 피해 없이 넉백이나 디버프만 줄 수 있습니다."))
	float DamageAmount = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap",
			ToolTip = "닿았을 때 둔화 디버프를 줄지 정합니다."))
	bool bApplySlowOnOverlap = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap && bApplySlowOnOverlap", ClampMin = "0.1", Units = "s"))
	float SlowDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap && bApplySlowOnOverlap", ClampMin = "0.1", ClampMax = "1.0"))
	float SlowMultiplier = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap",
			ToolTip = "닿았을 때 천복이를 밀어낼지 정합니다."))
	bool bApplyKnockbackOnOverlap = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap && bApplyKnockbackOnOverlap", ClampMin = "0.0"))
	float KnockbackStrength = 650.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap && bApplyKnockbackOnOverlap", ClampMin = "0.0"))
	float KnockbackUpwardStrength = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap && bApplyKnockbackOnOverlap",
			ToolTip = "켜면 방해물의 이동 방향으로 밀어냅니다. 끄면 방해물 중심에서 바깥쪽으로 밀어냅니다."))
	bool bUseMovementDirectionForKnockback = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Hazard",
		meta = (EditCondition = "bOverrideHazard && bAffectPlayerOnOverlap && bApplyKnockbackOnOverlap", ClampMin = "0.0", Units = "s"))
	float KnockbackCooldown = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (ToolTip = "켜면 경로 이동 값을 이 규칙으로 덮어씁니다."))
	bool bOverridePathMovement = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement", ToolTip = "켜면 방해물이 활성화 중 경로를 따라 움직입니다."))
	bool bMoveWhenActive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive", ClampMin = "0.0", Units = "cm/s"))
	float MovementSpeed = 160.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive",
			ToolTip = "켜면 배치된 위치를 첫 번째 경로 지점으로 사용합니다."))
	bool bUseInitialLocationAsFirstPathPoint = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive",
			ToolTip = "배치 위치 기준 상대 좌표 경로입니다. 일괄 관리 규칙은 레벨 액터 참조 대신 이 값을 사용합니다."))
	TArray<FVector> PathPointOffsets = { FVector(400.0f, 0.0f, 0.0f) };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive"))
	ECh03WaveEnvironmentPathEndBehavior PathEndBehavior =
		ECh03WaveEnvironmentPathEndBehavior::Reverse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive",
			ToolTip = "켜면 이동 방향을 바라봅니다."))
	bool bFaceMovementDirection = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive && bFaceMovementDirection", ClampMin = "0.0"))
	float FacingRotationInterpSpeed = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Path Movement",
		meta = (EditCondition = "bOverridePathMovement && bMoveWhenActive && bFaceMovementDirection"))
	FRotator FacingRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Rotation",
		meta = (ToolTip = "켜면 움직이는 부분의 회전 값을 이 규칙으로 덮어씁니다."))
	bool bOverrideRotatingPart = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Rotation",
		meta = (EditCondition = "bOverrideRotatingPart",
			ToolTip = "켜면 VisualMesh와 HazardVolume만 회전합니다. 고정부 메쉬는 회전하지 않습니다."))
	bool bRotateMovingPartWhenActive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Rotation",
		meta = (EditCondition = "bOverrideRotatingPart && bRotateMovingPartWhenActive"))
	ECh03WaveEnvironmentRotationAxis RotatingPartAxis =
		ECh03WaveEnvironmentRotationAxis::Z;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Rotation",
		meta = (EditCondition = "bOverrideRotatingPart && bRotateMovingPartWhenActive", Units = "deg/s",
			ToolTip = "초당 회전 각도입니다. 음수로 입력하면 반대로 돕니다."))
	float RotatingPartSpeed = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Managed|Rotation",
		meta = (EditCondition = "bOverrideRotatingPart",
			ToolTip = "비활성화될 때 회전부를 초기 각도로 되돌립니다."))
	bool bResetRotatingPartWhenInactive = true;
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

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Wave Environment|Managed")
	bool MatchesManagedRuleTag(FName RuleTag) const;

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Wave Environment|Managed")
	void ApplyManagedRule(const FCh03_WaveEnvironmentManagedRule& ManagedRule);

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
	TObjectPtr<USceneComponent> MovingRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	TObjectPtr<UBoxComponent> HazardVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment")
	TObjectPtr<UStaticMeshComponent> StationaryMesh;

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
		meta = (EditCondition = "bAffectPlayerOnOverlap", ClampMin = "0.05", Units = "s",
			ToolTip = "천복이가 방해물과 계속 닿아 있을 때 피해와 디버프를 다시 적용하는 간격입니다."))
	float ContactEffectInterval = 0.65f;

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
	void UpdateRotatingPart(float DeltaSeconds);
	void ProcessHazardOverlaps();
	void TryApplyHazardEffect(ACh03_CheonbokCharacter* CheonbokCharacter);
	bool CanApplyContactEffect() const;
	void DrawMovementPathDebug() const;
	void LogPathConfigurationWarningIfNeeded();
	FVector GetRotatingPartAxisVector() const;
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
	void ResetRotatingPart();
	void RefreshTickEnabled();

	int32 LastAppliedWave = 0;
	int32 LastAppliedMaxWave = 0;
	bool bIsEnvironmentActive = false;

	FVector InitialActorLocation = FVector::ZeroVector;
	FRotator InitialActorRotation = FRotator::ZeroRotator;
	FRotator InitialMovingRootRelativeRotation = FRotator::ZeroRotator;
	float LastHazardEffectTime = -BIG_NUMBER;
	float LastKnockbackTime = -BIG_NUMBER;
	FVector CurrentMovementDirection = FVector::ZeroVector;
	float PathTravelDistance = 0.0f;
	float PathTravelDirection = 1.0f;
	bool bHasCachedInitialLocation = false;
	bool bHasCachedInitialMovingRootRotation = false;
	bool bHasReachedPathEnd = false;
	bool bPathWarningLogged = false;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Rotation",
		meta = (ToolTip = "켜면 활성화 중 움직이는 부분만 회전합니다. VisualMesh와 HazardVolume이 회전하고 StationaryMesh는 고정됩니다."))
	bool bRotateMovingPartWhenActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Rotation",
		meta = (EditCondition = "bRotateMovingPartWhenActive",
			ToolTip = "움직이는 부분이 회전할 로컬 축입니다. 일반적인 회전목마는 Z축을 사용합니다."))
	ECh03WaveEnvironmentRotationAxis RotatingPartAxis =
		ECh03WaveEnvironmentRotationAxis::Z;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Rotation",
		meta = (EditCondition = "bRotateMovingPartWhenActive", Units = "deg/s",
			ToolTip = "초당 회전 각도입니다. 음수로 입력하면 반대로 돕니다."))
	float RotatingPartSpeed = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Rotation",
		meta = (ToolTip = "켜면 비활성화될 때 움직이는 부분의 각도를 초기 상태로 되돌립니다."))
	bool bResetRotatingPartWhenInactive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Debug",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "PIE 실행 중 경로를 선과 점으로 표시합니다. 최종 제출 전에는 꺼두는 것을 권장합니다."))
	bool bDrawMovementPathDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Debug",
		meta = (EditCondition = "bMoveWhenActive && bDrawMovementPathDebug"))
	FColor MovementPathDebugColor = FColor::Cyan;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Debug",
		meta = (EditCondition = "bMoveWhenActive && bDrawMovementPathDebug", Units = "cm"))
	float MovementPathDebugHeightOffset = 20.0f;
};
