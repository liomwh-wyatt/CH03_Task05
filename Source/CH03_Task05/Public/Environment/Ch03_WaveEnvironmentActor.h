#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ch03_WaveEnvironmentActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ACh03_CheonbokCharacter;

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

private:
	FVector GetKnockbackDirection(
		const ACh03_CheonbokCharacter* CheonbokCharacter) const;
	void CacheInitialLocationIfNeeded();
	void UpdateActiveMovement(float DeltaSeconds);
	void ResetMovement();

	int32 LastAppliedWave = 0;
	int32 LastAppliedMaxWave = 0;
	bool bIsEnvironmentActive = false;

	FVector InitialActorLocation = FVector::ZeroVector;
	float MovementAlpha = 0.0f;
	float MovementDirection = 1.0f;
	float LastKnockbackTime = -BIG_NUMBER;
	bool bHasCachedInitialLocation = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement")
	bool bMoveWhenActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive", ClampMin = "0.0", Units = "cm/s"))
	float MovementSpeed = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive"))
	FVector MovementOffset = FVector(400.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Wave Environment|Movement",
		meta = (EditCondition = "bMoveWhenActive"))
	bool bResetLocationWhenInactive = true;
};
