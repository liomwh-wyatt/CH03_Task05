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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰", meta = (DisplayName = "아이템 클래스"))
	TSubclassOf<ACh03_BaseItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰", meta = (DisplayName = "가중치", ClampMin = "0.0"))
	float Weight = 1.0f;
};

UCLASS()
class CH03_TASK05_API ACh03_SpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ACh03_SpawnVolume();

	UFUNCTION(BlueprintCallable, Category = "스폰|웨이브")
	void ApplyWaveSettings(
		const TArray<FCh03_SpawnItemEntry>& NewSpawnEntries,
		int32 NewInitialSpawnCount,
		int32 NewMaxAliveItems,
		float NewSpawnInterval);

	UFUNCTION(BlueprintCallable, Category = "스폰|웨이브")
	void SpawnInitialItems();

	UFUNCTION(BlueprintCallable, Category = "스폰")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "스폰")
	void StopSpawning();

	UFUNCTION(BlueprintCallable, Category = "스폰")
	void SpawnItems(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "스폰")
	ACh03_BaseItem* SpawnOneItem();

	UFUNCTION(BlueprintCallable, Category = "스폰")
	ACh03_BaseItem* SpawnItemOfClass(
		TSubclassOf<ACh03_BaseItem> ItemClass,
		bool bAllowExceedMaxAliveItems = true);

	UFUNCTION(BlueprintCallable, Category = "스폰")
	void ClearSpawnedItems();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스폰", meta = (DisplayName = "스폰 범위"))
	TObjectPtr<UBoxComponent> SpawnBounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|아이템 목록", meta = (DisplayName = "스폰 아이템 목록"))
	TArray<FCh03_SpawnItemEntry> SpawnEntries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|개수", meta = (DisplayName = "자동 시작"))
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|개수",
		meta = (DisplayName = "초기 스폰 수", ClampMin = "0"))
	int32 InitialSpawnCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|개수",
		meta = (DisplayName = "최대 활성 아이템 수", ClampMin = "1"))
	int32 MaxAliveItems = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|개수",
		meta = (DisplayName = "스폰 간격", ClampMin = "0.1", Units = "s"))
	float SpawnInterval = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|안전 검사",
		meta = (DisplayName = "위치 찾기 시도 횟수", ClampMin = "1"))
	int32 MaxLocationAttempts = 20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|안전 검사",
		meta = (DisplayName = "플레이어 제외 반경", ClampMin = "0.0", Units = "cm"))
	float PlayerExclusionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|안전 검사",
		meta = (DisplayName = "아이템 최소 간격", ClampMin = "0.0", Units = "cm"))
	float MinimumItemSpacing = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|안전 검사",
		meta = (DisplayName = "스폰 여유 반경", ClampMin = "1.0", Units = "cm"))
	float SpawnClearanceRadius = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|충돌 검사",
		meta = (DisplayName = "바닥 탐색 시작 높이", ClampMin = "0.0", Units = "cm"))
	float GroundTraceHeight = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|충돌 검사",
		meta = (DisplayName = "바닥 탐색 깊이", ClampMin = "1.0", Units = "cm"))
	float GroundTraceDepth = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "스폰|디버그", meta = (DisplayName = "디버그 표시"))
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
