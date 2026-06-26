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
	Reverse UMETA(DisplayName = "Reverse At Ends"),
	Loop UMETA(DisplayName = "Loop To First Point"),
	Stop UMETA(DisplayName = "Stop At End")
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
		meta = (ClampMin = "0", ToolTip = "0 means the actor stays available through the last wave."))
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
	void AdvancePathTarget();
	int32 GetPathPointCount() const;
	FVector GetPathPointWorldLocation(int32 PointIndex) const;
	bool HasValidPathPointActors() const;
	int32 GetValidPathPointActorCount() const;
	FVector GetValidPathPointActorLocation(int32 ValidPathPointIndex) const;
	void ResetMovement();

	int32 LastAppliedWave = 0;
	int32 LastAppliedMaxWave = 0;
	bool bIsEnvironmentActive = false;

	FVector InitialActorLocation = FVector::ZeroVector;
	float LastKnockbackTime = -BIG_NUMBER;
	FVector CurrentMovementDirection = FVector::ZeroVector;
	int32 CurrentPathPointIndex = 0;
	int32 PathDirection = 1;
	bool bHasCachedInitialLocation = false;
	bool bHasReachedPathEnd = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement")
	bool bMoveWhenActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive", ClampMin = "0.0", Units = "cm/s",
			ToolTip = "Speed used while following the configured path."))
	float MovementSpeed = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "If true, the actor's placed location becomes the first path point."))
	bool bUseInitialLocationAsFirstPathPoint = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "Optional level actors used as path points. If any valid actor is assigned here, Path Point Offsets are ignored."))
	TArray<TObjectPtr<AActor>> PathPointActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "Fallback path points relative to the actor's placed location. Used only when no valid Path Point Actors are assigned."))
	TArray<FVector> PathPointOffsets = { FVector(400.0f, 0.0f, 0.0f) };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive", ClampMin = "1.0", Units = "cm",
			ToolTip = "Distance at which the actor treats the current path point as reached."))
	float PathPointAcceptanceRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement|Path",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "How the actor behaves after reaching the last path point."))
	ECh03WaveEnvironmentPathEndBehavior PathEndBehavior =
		ECh03WaveEnvironmentPathEndBehavior::Reverse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive",
			ToolTip = "If true, the actor returns to its placed location when it becomes inactive."))
	bool bResetLocationWhenInactive = true;
};
