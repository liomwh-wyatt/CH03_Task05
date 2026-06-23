#include "Items/Ch03_GoldenChuruItem.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ACh03_GoldenChuruItem::ACh03_GoldenChuruItem()
{
	ItemType = TEXT("GoldenChuru");
	ScoreValue = 450;
	RotationSpeed = 220.0f;
	BobAmplitude = 0.0f;
	BobFrequency = 0.0f;
	LifetimeAfterSpawn = 14.0f;
	BlinkStartTime = 3.0f;
	BlinkInterval = 0.1f;
}

void ACh03_GoldenChuruItem::BeginPlay()
{
	Super::BeginPlay();

	CurrentFlutterDirection = GetActorForwardVector().GetSafeNormal();
	if (CurrentFlutterDirection.IsNearlyZero())
	{
		CurrentFlutterDirection = FVector::ForwardVector;
	}

	SelectNewFlutterTarget();
}

void ACh03_GoldenChuruItem::UpdateItemMovement(
	const float DeltaTime)
{
	if (!bUseFlutterMovement || DeltaTime <= 0.0f)
	{
		Super::UpdateItemMovement(DeltaTime);
		return;
	}

	AddActorLocalRotation(
		FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));

	const FVector CurrentLocation = GetActorLocation();
	const float DistanceToTarget =
		FVector::Dist(CurrentLocation, CurrentFlutterTarget);

	FlutterTargetChangeTimer -= DeltaTime;
	if (FlutterTargetChangeTimer <= 0.0f
		|| DistanceToTarget <= FlutterTargetAcceptanceRadius)
	{
		SelectNewFlutterTarget();
	}

	FVector DesiredDirection =
		(CurrentFlutterTarget - CurrentLocation).GetSafeNormal();
	if (DesiredDirection.IsNearlyZero())
	{
		DesiredDirection = CurrentFlutterDirection;
	}

	const FVector RightNoise =
		FVector::CrossProduct(FVector::UpVector, DesiredDirection)
		.GetSafeNormal();
	const float NoisePhase = GetRunningTime() * 2.0f * PI;
	const FVector NoisyDirection =
		RightNoise * FMath::Sin(NoisePhase * 2.4f) * FlutterDirectionNoise
		+ FVector::UpVector
			* FMath::Cos(NoisePhase * 3.1f)
			* FlutterDirectionNoise
			* 0.45f;

	float SpeedMultiplier = 1.0f;
	if (const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		const FVector FromPlayer =
			CurrentLocation - PlayerPawn->GetActorLocation();
		const float DistSquared2D = FromPlayer.SizeSquared2D();
		if (DistSquared2D < FMath::Square(PlayerEscapeRadius))
		{
			const FVector EscapeDirection =
				FVector(FromPlayer.X, FromPlayer.Y, 0.0f).GetSafeNormal();
			DesiredDirection =
				(DesiredDirection + EscapeDirection * 0.85f).GetSafeNormal();
			SpeedMultiplier = PlayerEscapeSpeedMultiplier;
		}
	}

	const FVector TargetDirection =
		(DesiredDirection + NoisyDirection).GetSafeNormal();

	CurrentFlutterDirection = FMath::VInterpTo(
		CurrentFlutterDirection,
		TargetDirection.IsNearlyZero()
			? DesiredDirection
			: TargetDirection,
		DeltaTime,
		FlutterTurnSharpness).GetSafeNormal();

	if (CurrentFlutterDirection.IsNearlyZero())
	{
		CurrentFlutterDirection = DesiredDirection;
	}

	const FVector NewLocation = ClampToFlutterBounds(
		CurrentLocation
		+ CurrentFlutterDirection
			* FlutterMoveSpeed
			* SpeedMultiplier
			* DeltaTime);

	SetActorLocation(NewLocation);
}

void ACh03_GoldenChuruItem::SelectNewFlutterTarget()
{
	CurrentFlutterTarget = GetRandomFlutterLocation();

	const float MinInterval = FMath::Max(
		0.05f,
		FlutterTargetChangeIntervalMin);
	const float MaxInterval = FMath::Max(
		MinInterval,
		FlutterTargetChangeIntervalMax);

	FlutterTargetChangeTimer = FMath::FRandRange(
		MinInterval,
		MaxInterval);
}

FVector ACh03_GoldenChuruItem::GetRandomFlutterLocation() const
{
	const FVector Origin = GetInitialLocation();
	const FVector SafeExtent(
		FMath::Max(0.0f, FlutterBoundsExtent.X),
		FMath::Max(0.0f, FlutterBoundsExtent.Y),
		FMath::Max(0.0f, FlutterBoundsExtent.Z));

	return Origin + FVector(
		FMath::FRandRange(-SafeExtent.X, SafeExtent.X),
		FMath::FRandRange(-SafeExtent.Y, SafeExtent.Y),
		FMath::FRandRange(-SafeExtent.Z, SafeExtent.Z));
}

FVector ACh03_GoldenChuruItem::ClampToFlutterBounds(
	const FVector& Location) const
{
	const FVector Origin = GetInitialLocation();
	const FVector SafeExtent(
		FMath::Max(0.0f, FlutterBoundsExtent.X),
		FMath::Max(0.0f, FlutterBoundsExtent.Y),
		FMath::Max(0.0f, FlutterBoundsExtent.Z));

	return FVector(
		FMath::Clamp(
			Location.X,
			Origin.X - SafeExtent.X,
			Origin.X + SafeExtent.X),
		FMath::Clamp(
			Location.Y,
			Origin.Y - SafeExtent.Y,
			Origin.Y + SafeExtent.Y),
		FMath::Clamp(
			Location.Z,
			Origin.Z - SafeExtent.Z,
			Origin.Z + SafeExtent.Z));
}
