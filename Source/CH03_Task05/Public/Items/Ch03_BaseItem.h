// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Ch03_ItemInterface.h"
#include "Ch03_BaseItem.generated.h"

class USceneComponent;
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

	virtual void OnItemOverlap_Implementation(AActor* OverlapActor) override;
	virtual void OnItemEndOverlap_Implementation(AActor* OverlapActor) override;
	virtual void ActivateItem_Implementation(AActor* Activator) override;
	virtual FName GetItemType_Implementation() const override;

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

	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Feedback")
	void OnCollected(AActor* Activator);

	bool CanBeActivatedBy(const AActor* Activator) const;
	bool IsConsumed() const { return bIsConsumed; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Movement",
		meta = (ClampMin = "0.0"))
	float RotationSpeed = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Movement",
		meta = (ClampMin = "0.0"))
	float BobAmplitude = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Movement",
		meta = (ClampMin = "0.0"))
	float BobFrequency = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Feedback")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Feedback",
		meta = (ClampMin = "0.0"))
	float DestroyDelay = 0.05f;

private:
	bool bIsConsumed = false;
	FVector InitialLocation = FVector::ZeroVector;
	float RunningTime = 0.0f;
};
