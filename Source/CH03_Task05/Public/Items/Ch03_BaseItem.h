// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Ch03_ItemInterface.h"
#include "Ch03_BaseItem.generated.h"

class USceneComponent;
class UNiagaraSystem;
class USoundBase;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class CH03_TASK05_API ACh03_BaseItem : public AActor, public ICh03_ItemInterface
{
	GENERATED_BODY()
	
public:	
	ACh03_BaseItem();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void UpdateItemMovement(float DeltaTime);

	virtual void OnItemOverlap_Implementation(AActor* OverlapActor) override;
	virtual void OnItemEndOverlap_Implementation(AActor* OverlapActor) override;
	virtual void ActivateItem_Implementation(AActor* Activator) override;
	virtual FName GetItemType_Implementation() const override;
	virtual void PlaySpawnFeedback();
	virtual void PlayPickupFeedback(AActor* Activator);

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "아이템|피드백")
	void OnCollected(AActor* Activator);

	bool CanBeActivatedBy(const AActor* Activator) const;
	bool IsConsumed() const { return bIsConsumed; }
	const FVector& GetInitialLocation() const { return InitialLocation; }
	float GetRunningTime() const { return RunningTime; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "아이템|컴포넌트", meta = (DisplayName = "씬 루트"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "아이템|컴포넌트", meta = (DisplayName = "충돌 컴포넌트"))
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "아이템|컴포넌트", meta = (DisplayName = "메쉬 컴포넌트"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템", meta = (DisplayName = "아이템 타입"))
	FName ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|이동",
		meta = (DisplayName = "회전 속도", ClampMin = "0.0"))
	float RotationSpeed = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|이동",
		meta = (DisplayName = "떠오르기 진폭", ClampMin = "0.0"))
	float BobAmplitude = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|이동",
		meta = (DisplayName = "떠오르기 빈도", ClampMin = "0.0"))
	float BobFrequency = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|피드백", meta = (DisplayName = "획득 사운드"))
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|피드백", meta = (DisplayName = "생성 이펙트"))
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|피드백", meta = (DisplayName = "획득 이펙트"))
	TObjectPtr<UNiagaraSystem> PickupEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|피드백",
		meta = (DisplayName = "제거 지연", ClampMin = "0.0"))
	float DestroyDelay = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|수명", meta = (DisplayName = "생성 후 만료"))
	bool bExpireAfterSpawn = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|수명",
		meta = (DisplayName = "생성 후 수명", EditCondition = "bExpireAfterSpawn", ClampMin = "0.1", Units = "s"))
	float LifetimeAfterSpawn = 14.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|수명", meta = (DisplayName = "만료 전 깜박임"))
	bool bBlinkBeforeExpire = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|수명",
		meta = (DisplayName = "깜박임 시작 시간", EditCondition = "bBlinkBeforeExpire", ClampMin = "0.0", Units = "s"))
	float BlinkStartTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|수명",
		meta = (DisplayName = "깜박임 간격", EditCondition = "bBlinkBeforeExpire", ClampMin = "0.03", Units = "s"))
	float BlinkInterval = 0.18f;

private:
	void UpdateLifetime(float DeltaTime);
	void ExpireItem();

	bool bIsConsumed = false;
	FVector InitialLocation = FVector::ZeroVector;
	float RunningTime = 0.0f;
	float SpawnLifeElapsed = 0.0f;
	float BlinkElapsed = 0.0f;
	bool bIsBlinkVisible = true;
};
