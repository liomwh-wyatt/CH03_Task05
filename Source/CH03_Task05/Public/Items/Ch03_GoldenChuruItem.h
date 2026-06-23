#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_ScoreItem.h"
#include "Ch03_GoldenChuruItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_GoldenChuruItem : public ACh03_ScoreItem
{
	GENERATED_BODY()

public:
	ACh03_GoldenChuruItem();

protected:
	virtual void BeginPlay() override;
	virtual void UpdateItemMovement(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter")
	bool bUseFlutterMovement = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement"))
	FVector FlutterBoundsExtent = FVector(900.0f, 650.0f, 180.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "0.0", Units = "cm/s"))
	float FlutterMoveSpeed = 420.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "0.0"))
	float FlutterTurnSharpness = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "1.0", Units = "cm"))
	float FlutterTargetAcceptanceRadius = 95.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "0.05", Units = "s"))
	float FlutterTargetChangeIntervalMin = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "0.05", Units = "s"))
	float FlutterTargetChangeIntervalMax = 0.9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "0.0"))
	float FlutterDirectionNoise = 0.42f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "0.0", Units = "cm"))
	float PlayerEscapeRadius = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter",
		meta = (EditCondition = "bUseFlutterMovement", ClampMin = "1.0"))
	float PlayerEscapeSpeedMultiplier = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter|Ground")
	bool bKeepAboveGround = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter|Ground",
		meta = (EditCondition = "bKeepAboveGround", ClampMin = "0.0", Units = "cm"))
	float GroundClearance = 95.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter|Ground",
		meta = (EditCondition = "bKeepAboveGround", ClampMin = "0.0", Units = "cm"))
	float GroundTraceHeight = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flutter|Ground",
		meta = (EditCondition = "bKeepAboveGround", ClampMin = "1.0", Units = "cm"))
	float GroundTraceDepth = 1200.0f;

private:
	void SelectNewFlutterTarget();
	FVector GetRandomFlutterLocation() const;
	FVector ClampToFlutterBounds(const FVector& Location) const;
	FVector KeepLocationAboveGround(const FVector& Location) const;

	FVector CurrentFlutterTarget = FVector::ZeroVector;
	FVector CurrentFlutterDirection = FVector::ForwardVector;
	float FlutterTargetChangeTimer = 0.0f;
};
