#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Items/Ch03_ScoreItem.h"
#include "Ch03_GoldenChuruItem.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class CH03_TASK05_API ACh03_GoldenChuruItem : public ACh03_ScoreItem
{
	GENERATED_BODY()

public:
	ACh03_GoldenChuruItem();

protected:
	virtual void BeginPlay() override;
	virtual void UpdateItemMovement(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동", meta = (DisplayName = "날갯짓 이동 사용"))
	bool bUseFlutterMovement = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "아이템|컴포넌트", meta = (DisplayName = "비행 궤적 컴포넌트"))
	TObjectPtr<UNiagaraComponent> FlightTrailComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|이펙트", meta = (DisplayName = "비행 궤적 이펙트"))
	TObjectPtr<UNiagaraSystem> FlightTrailEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|이펙트", meta = (DisplayName = "비행 궤적 자동 활성화"))
	bool bAutoActivateFlightTrail = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "날갯짓 이동 범위", EditCondition = "bUseFlutterMovement"))
	FVector FlutterBoundsExtent = FVector(900.0f, 650.0f, 70.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "날갯짓 이동 속도", EditCondition = "bUseFlutterMovement", ClampMin = "0.0", Units = "cm/s"))
	float FlutterMoveSpeed = 420.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "날갯짓 회전 민감도", EditCondition = "bUseFlutterMovement", ClampMin = "0.0"))
	float FlutterTurnSharpness = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "목표 도착 허용 반경", EditCondition = "bUseFlutterMovement", ClampMin = "1.0", Units = "cm"))
	float FlutterTargetAcceptanceRadius = 95.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "목표 변경 최소 간격", EditCondition = "bUseFlutterMovement", ClampMin = "0.05", Units = "s"))
	float FlutterTargetChangeIntervalMin = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "목표 변경 최대 간격", EditCondition = "bUseFlutterMovement", ClampMin = "0.05", Units = "s"))
	float FlutterTargetChangeIntervalMax = 0.9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "이동 방향 노이즈", EditCondition = "bUseFlutterMovement", ClampMin = "0.0"))
	float FlutterDirectionNoise = 0.42f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "플레이어 회피 반경", EditCondition = "bUseFlutterMovement", ClampMin = "0.0", Units = "cm"))
	float PlayerEscapeRadius = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동",
		meta = (DisplayName = "플레이어 회피 속도 배율", EditCondition = "bUseFlutterMovement", ClampMin = "1.0"))
	float PlayerEscapeSpeedMultiplier = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|바닥 보정", meta = (DisplayName = "바닥 위 유지"))
	bool bKeepAboveGround = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|바닥 보정",
		meta = (DisplayName = "바닥 여유 높이", EditCondition = "bKeepAboveGround", ClampMin = "0.0", Units = "cm"))
	float GroundClearance = 80.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|바닥 보정",
		meta = (DisplayName = "최대 바닥 여유 높이", EditCondition = "bKeepAboveGround", ClampMin = "0.0", Units = "cm"))
	float MaxGroundClearance = 135.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|바닥 보정",
		meta = (DisplayName = "바닥 탐색 시작 높이", EditCondition = "bKeepAboveGround", ClampMin = "0.0", Units = "cm"))
	float GroundTraceHeight = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|바닥 보정",
		meta = (DisplayName = "바닥 탐색 깊이", EditCondition = "bKeepAboveGround", ClampMin = "1.0", Units = "cm"))
	float GroundTraceDepth = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|장애물 회피", meta = (DisplayName = "월드 장애물 회피"))
	bool bAvoidWorldObstacles = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|장애물 회피",
		meta = (DisplayName = "장애물 탐색 채널", EditCondition = "bAvoidWorldObstacles"))
	TEnumAsByte<ECollisionChannel> ObstacleTraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|장애물 회피",
		meta = (DisplayName = "장애물 회피 반경", EditCondition = "bAvoidWorldObstacles", ClampMin = "1.0", Units = "cm"))
	float ObstacleAvoidanceRadius = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|날갯짓 이동|장애물 회피",
		meta = (DisplayName = "목표 위치 시도 횟수", EditCondition = "bAvoidWorldObstacles", ClampMin = "1"))
	int32 FlutterTargetAttemptCount = 12;

private:
	void SelectNewFlutterTarget();
	FVector GetRandomFlutterLocation() const;
	FVector ClampToFlutterBounds(const FVector& Location) const;
	FVector KeepLocationAboveGround(const FVector& Location) const;
	bool IsFlutterLocationBlocked(const FVector& Location) const;
	bool IsFlutterPathBlocked(
		const FVector& StartLocation,
		const FVector& EndLocation,
		FHitResult& OutHit) const;

	FVector CurrentFlutterTarget = FVector::ZeroVector;
	FVector CurrentFlutterDirection = FVector::ForwardVector;
	FVector FlutterOriginLocation = FVector::ZeroVector;
	float FlutterTargetChangeTimer = 0.0f;
};
