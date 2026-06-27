#include "Items/Ch03_GoldenChuruItem.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

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

	FlightTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(
		TEXT("FlightTrailComponent"));
	FlightTrailComponent->SetupAttachment(SceneRoot);
	FlightTrailComponent->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundFinder(
		TEXT("/Game/Audio/SFX/Items/S_GoldenWingSnack_Pickup.S_GoldenWingSnack_Pickup"));
	if (PickupSoundFinder.Succeeded())
	{
		PickupSound = PickupSoundFinder.Object;
	}
}

void ACh03_GoldenChuruItem::BeginPlay()
{
	Super::BeginPlay();

	CurrentFlutterDirection = GetActorForwardVector().GetSafeNormal();
	if (CurrentFlutterDirection.IsNearlyZero())
	{
		CurrentFlutterDirection = FVector::ForwardVector;
	}

	SetActorLocation(KeepLocationAboveGround(GetActorLocation()));
	FlutterOriginLocation = GetActorLocation();

	if (FlightTrailComponent)
	{
		if (FlightTrailEffect)
		{
			FlightTrailComponent->SetAsset(FlightTrailEffect);
		}

		if (bUseFlutterMovement && bAutoActivateFlightTrail)
		{
			FlightTrailComponent->Activate(true);
		}
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

	FHitResult ObstacleHit;
	if (IsFlutterPathBlocked(CurrentLocation, NewLocation, ObstacleHit))
	{
		FVector EscapeDirection = ObstacleHit.ImpactNormal;
		EscapeDirection.Z = 0.0f;

		if (EscapeDirection.IsNearlyZero())
		{
			EscapeDirection = -CurrentFlutterDirection;
			EscapeDirection.Z = 0.0f;
		}

		CurrentFlutterDirection = EscapeDirection.GetSafeNormal();
		if (CurrentFlutterDirection.IsNearlyZero())
		{
			CurrentFlutterDirection = FVector::ForwardVector;
		}

		SelectNewFlutterTarget();
		return;
	}

	SetActorLocation(NewLocation);
}

void ACh03_GoldenChuruItem::SelectNewFlutterTarget()
{
	const int32 AttemptCount = FMath::Max(1, FlutterTargetAttemptCount);
	const FVector CurrentLocation = GetActorLocation();
	FVector FallbackTarget = CurrentLocation;
	bool bFoundOpenTarget = false;

	for (int32 AttemptIndex = 0; AttemptIndex < AttemptCount; ++AttemptIndex)
	{
		const FVector CandidateLocation = GetRandomFlutterLocation();
		FallbackTarget = CandidateLocation;

		if (!IsFlutterLocationBlocked(CandidateLocation))
		{
			CurrentFlutterTarget = CandidateLocation;
			bFoundOpenTarget = true;
			break;
		}
	}

	if (!bFoundOpenTarget)
	{
		CurrentFlutterTarget = FallbackTarget;
	}

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
	const FVector Origin = FlutterOriginLocation;
	const FVector SafeExtent(
		FMath::Max(0.0f, FlutterBoundsExtent.X),
		FMath::Max(0.0f, FlutterBoundsExtent.Y),
		FMath::Max(0.0f, FlutterBoundsExtent.Z));

	return ClampToFlutterBounds(
		Origin + FVector(
			FMath::FRandRange(-SafeExtent.X, SafeExtent.X),
			FMath::FRandRange(-SafeExtent.Y, SafeExtent.Y),
			FMath::FRandRange(0.0f, SafeExtent.Z)));
}

FVector ACh03_GoldenChuruItem::ClampToFlutterBounds(
	const FVector& Location) const
{
	const FVector Origin = FlutterOriginLocation;
	const FVector SafeExtent(
		FMath::Max(0.0f, FlutterBoundsExtent.X),
		FMath::Max(0.0f, FlutterBoundsExtent.Y),
		FMath::Max(0.0f, FlutterBoundsExtent.Z));

	const FVector ClampedLocation(
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
			Origin.Z,
			Origin.Z + SafeExtent.Z));

	return KeepLocationAboveGround(ClampedLocation);
}

FVector ACh03_GoldenChuruItem::KeepLocationAboveGround(
	const FVector& Location) const
{
	if (!bKeepAboveGround || !GetWorld())
	{
		return Location;
	}

	const FVector TraceStart =
		Location + FVector::UpVector * GroundTraceHeight;
	const FVector TraceEnd =
		Location - FVector::UpVector * GroundTraceDepth;

	FHitResult GroundHit;
	FCollisionQueryParams TraceParams(
		SCENE_QUERY_STAT(Ch03GoldenChuruGroundTrace),
		false,
		this);
	TraceParams.AddIgnoredActor(this);

	const bool bHitGround = GetWorld()->LineTraceSingleByChannel(
		GroundHit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		TraceParams);

	if (!bHitGround || !GroundHit.bBlockingHit)
	{
		return Location;
	}

	FVector AdjustedLocation = Location;
	const float MinimumZ = GroundHit.ImpactPoint.Z + GroundClearance;
	const float MaximumZ = GroundHit.ImpactPoint.Z + FMath::Max(
		GroundClearance,
		MaxGroundClearance);
	if (AdjustedLocation.Z < MinimumZ)
	{
		AdjustedLocation.Z = MinimumZ;
	}
	else if (AdjustedLocation.Z > MaximumZ)
	{
		AdjustedLocation.Z = MaximumZ;
	}

	return AdjustedLocation;
}

bool ACh03_GoldenChuruItem::IsFlutterLocationBlocked(
	const FVector& Location) const
{
	if (!bAvoidWorldObstacles || !GetWorld() || ObstacleAvoidanceRadius <= 0.0f)
	{
		return false;
	}

	FCollisionQueryParams QueryParams(
		SCENE_QUERY_STAT(Ch03GoldenChuruObstacleOverlap),
		false,
		this);
	QueryParams.AddIgnoredActor(this);

	const FCollisionShape ProbeShape =
		FCollisionShape::MakeSphere(ObstacleAvoidanceRadius);

	return GetWorld()->OverlapBlockingTestByChannel(
		Location,
		FQuat::Identity,
		ObstacleTraceChannel,
		ProbeShape,
		QueryParams);
}

bool ACh03_GoldenChuruItem::IsFlutterPathBlocked(
	const FVector& StartLocation,
	const FVector& EndLocation,
	FHitResult& OutHit) const
{
	if (!bAvoidWorldObstacles
		|| !GetWorld()
		|| ObstacleAvoidanceRadius <= 0.0f
		|| FVector::DistSquared(StartLocation, EndLocation) <= KINDA_SMALL_NUMBER)
	{
		return false;
	}

	FCollisionQueryParams QueryParams(
		SCENE_QUERY_STAT(Ch03GoldenChuruObstacleSweep),
		false,
		this);
	QueryParams.AddIgnoredActor(this);

	const FCollisionShape ProbeShape =
		FCollisionShape::MakeSphere(ObstacleAvoidanceRadius);

	return GetWorld()->SweepSingleByChannel(
		OutHit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ObstacleTraceChannel,
		ProbeShape,
		QueryParams);
}
