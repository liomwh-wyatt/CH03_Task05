// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ch03_SpawnVolume.generated.h"

class ACh03_BaseItem;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FCh03_SpawnItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ACh03_BaseItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;
};

UCLASS()
class CH03_TASK05_API ACh03_SpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ACh03_SpawnVolume();

	UFUNCTION(BlueprintCallable, Category = "Spawn|Wave")
	void ApplyWaveSettings(
		const TArray<FCh03_SpawnItemEntry>& NewSpawnEntries,
		int32 NewInitialSpawnCount,
		int32 NewMaxAliveItems,
		float NewSpawnInterval);

	UFUNCTION(BlueprintCallable, Category = "Spawn|Wave")
	void SpawnInitialItems();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StopSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnItems(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	ACh03_BaseItem* SpawnOneItem();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	ACh03_BaseItem* SpawnItemOfClass(
		TSubclassOf<ACh03_BaseItem> ItemClass,
		bool bAllowExceedMaxAliveItems = true);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void ClearSpawnedItems();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> SpawnBounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Items")
	TArray<FCh03_SpawnItemEntry> SpawnEntries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Population")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Population",
		meta = (ClampMin = "0"))
	int32 InitialSpawnCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Population",
		meta = (ClampMin = "1"))
	int32 MaxAliveItems = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Population",
		meta = (ClampMin = "0.1", Units = "s"))
	float SpawnInterval = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Safety",
		meta = (ClampMin = "1"))
	int32 MaxLocationAttempts = 20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Safety",
		meta = (ClampMin = "0.0", Units = "cm"))
	float PlayerExclusionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Safety",
		meta = (ClampMin = "0.0", Units = "cm"))
	float MinimumItemSpacing = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Safety",
		meta = (ClampMin = "1.0", Units = "cm"))
	float SpawnClearanceRadius = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Trace",
		meta = (ClampMin = "0.0", Units = "cm"))
	float GroundTraceHeight = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Trace",
		meta = (ClampMin = "1.0", Units = "cm"))
	float GroundTraceDepth = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Debug")
	bool bDrawDebug = false;

private:
	void MaintainSpawnPopulation();
	void PruneInvalidItems();
	bool FindValidSpawnLocation(FVector& OutLocation) const;
	bool IsLocationSafe(const FVector& Location) const;
	TSubclassOf<ACh03_BaseItem> SelectWeightedItemClass() const;

	UFUNCTION()
	void HandleSpawnedItemDestroyed(AActor* DestroyedActor);

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACh03_BaseItem>> SpawnedItems;

	FTimerHandle SpawnTimerHandle;
};
