#include "Environment/Ch03_WaveEnvironmentActor.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Ch03_GameStateBase.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ACh03_WaveEnvironmentActor::ACh03_WaveEnvironmentActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	HazardVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("HazardVolume"));
	HazardVolume->SetupAttachment(SceneRoot);
	HazardVolume->SetBoxExtent(FVector(75.0f, 75.0f, 45.0f));
	HazardVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HazardVolume->SetCollisionObjectType(ECC_WorldDynamic);
	HazardVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	HazardVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HazardVolume->SetGenerateOverlapEvents(true);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(SceneRoot);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 0.25f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeMesh.Object);
	}

	ActiveAnnouncementText = NSLOCTEXT(
		"CheonbokEnvironment",
		"DefaultEnvironmentAnnouncement",
		"New hazard is active!");
}

void ACh03_WaveEnvironmentActor::BeginPlay()
{
	Super::BeginPlay();

	CacheInitialLocationIfNeeded();

	HazardVolume->OnComponentBeginOverlap.AddUniqueDynamic(
		this,
		&ACh03_WaveEnvironmentActor::HandleHazardBeginOverlap);

	SetEnvironmentActive(false);
}

void ACh03_WaveEnvironmentActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsEnvironmentActive || !bMoveWhenActive)
	{
		return;
	}

	UpdateActiveMovement(DeltaSeconds);
}

bool ACh03_WaveEnvironmentActor::ApplyWaveState(
	const int32 CurrentWave,
	const int32 MaxWave)
{
	LastAppliedWave = CurrentWave;
	LastAppliedMaxWave = MaxWave;
	const bool bWasActive = bIsEnvironmentActive;
	SetEnvironmentActive(IsActiveForWave(CurrentWave));

	return !bWasActive && bIsEnvironmentActive;
}

void ACh03_WaveEnvironmentActor::SetEnvironmentActive(
	const bool bNewActive)
{
	const bool bWasActive = bIsEnvironmentActive;

	bIsEnvironmentActive = bNewActive;

	const bool bShouldBeVisible =
		bIsEnvironmentActive || !bHideWhenInactive;

	SetActorHiddenInGame(!bShouldBeVisible);
	SetActorEnableCollision(bIsEnvironmentActive);
	VisualMesh->SetVisibility(bShouldBeVisible, true);

	HazardVolume->SetCollisionEnabled(
		bIsEnvironmentActive
			? ECollisionEnabled::QueryOnly
			: ECollisionEnabled::NoCollision);

	SetActorTickEnabled(bIsEnvironmentActive && bMoveWhenActive);

	if (bWasActive == bIsEnvironmentActive)
	{
		return;
	}

	if (bIsEnvironmentActive)
	{
		UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtActor(
			this,
			ActivationFeedback,
			this);
		OnEnvironmentActivated(LastAppliedWave, LastAppliedMaxWave);
	}
	else
	{
		if (bResetLocationWhenInactive)
		{
			ResetMovement();
		}

		UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtActor(
			this,
			DeactivationFeedback,
			this);
		OnEnvironmentDeactivated(LastAppliedWave, LastAppliedMaxWave);
	}
}

bool ACh03_WaveEnvironmentActor::IsActiveForWave(
	const int32 CurrentWave) const
{
	if (CurrentWave < ActivateFromWave)
	{
		return false;
	}

	return DeactivateAfterWave <= 0
		|| CurrentWave <= DeactivateAfterWave;
}

void ACh03_WaveEnvironmentActor::HandleHazardBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bIsEnvironmentActive || !bAffectPlayerOnOverlap)
	{
		return;
	}

	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(OtherActor);
	if (!CheonbokCharacter || CheonbokCharacter->IsDead())
	{
		return;
	}

	if (ACh03_GameStateBase* CheonbokGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr)
	{
		CheonbokGameState->BreakComboWithReason(
			ECh03ComboBreakReason::Hazard);
	}

	float AppliedDamage = 0.0f;
	if (DamageAmount > 0.0f)
	{
		AppliedDamage = UGameplayStatics::ApplyDamage(
			CheonbokCharacter,
			DamageAmount,
			nullptr,
			this,
			nullptr);
	}

	UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtActor(
		this,
		HazardHitFeedback,
		CheonbokCharacter);
	OnHazardHitFeedback(CheonbokCharacter, AppliedDamage);

	if (CheonbokCharacter->IsDead())
	{
		return;
	}

	if (bApplySlowOnOverlap)
	{
		CheonbokCharacter->ApplySlow(
			SlowDuration,
			SlowMultiplier);
	}

	if (bApplyKnockbackOnOverlap
		&& (KnockbackStrength > 0.0f || KnockbackUpwardStrength > 0.0f))
	{
		const float CurrentTime =
			GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
		const bool bCanApplyKnockback =
			KnockbackCooldown <= 0.0f
			|| CurrentTime - LastKnockbackTime >= KnockbackCooldown;

		if (bCanApplyKnockback)
		{
			LastKnockbackTime = CurrentTime;
			CheonbokCharacter->ApplyKnockback(
				GetKnockbackDirection(CheonbokCharacter),
				KnockbackStrength,
				KnockbackUpwardStrength);
		}
	}
}

FVector ACh03_WaveEnvironmentActor::GetKnockbackDirection(
	const ACh03_CheonbokCharacter* CheonbokCharacter) const
{
	if (!CheonbokCharacter)
	{
		return FVector::ZeroVector;
	}

	if (bUseMovementDirectionForKnockback && bMoveWhenActive)
	{
		if (!CurrentMovementDirection.IsNearlyZero())
		{
			return CurrentMovementDirection;
		}
	}

	FVector KnockbackDirection =
		CheonbokCharacter->GetActorLocation() - GetActorLocation();
	KnockbackDirection.Z = 0.0f;

	if (KnockbackDirection.IsNearlyZero() && HazardVolume)
	{
		KnockbackDirection =
			CheonbokCharacter->GetActorLocation()
			- HazardVolume->GetComponentLocation();
		KnockbackDirection.Z = 0.0f;
	}

	return KnockbackDirection.GetSafeNormal();
}

void ACh03_WaveEnvironmentActor::CacheInitialLocationIfNeeded()
{
	if (bHasCachedInitialLocation)
	{
		return;
	}

	InitialActorLocation = GetActorLocation();
	InitialActorRotation = GetActorRotation();
	bHasCachedInitialLocation = true;
}

void ACh03_WaveEnvironmentActor::UpdateActiveMovement(
	const float DeltaSeconds)
{
	UpdatePathMovement(DeltaSeconds);
}

void ACh03_WaveEnvironmentActor::UpdatePathMovement(
	const float DeltaSeconds)
{
	CacheInitialLocationIfNeeded();

	const float PathTotalLength = GetPathTotalLength();
	if (PathTotalLength <= KINDA_SMALL_NUMBER
		|| MovementSpeed <= 0.0f
		|| bHasReachedPathEnd)
	{
		CurrentMovementDirection = FVector::ZeroVector;
		return;
	}

	const FVector PreviousLocation = GetActorLocation();
	float NextTravelDistance =
		PathTravelDistance
		+ MovementSpeed * DeltaSeconds * PathTravelDirection;

	int32 SafetyCounter = 0;
	while ((NextTravelDistance < 0.0f || NextTravelDistance > PathTotalLength)
		&& SafetyCounter < 8)
	{
		++SafetyCounter;

		if (PathEndBehavior == ECh03WaveEnvironmentPathEndBehavior::Reverse)
		{
			if (NextTravelDistance > PathTotalLength)
			{
				NextTravelDistance =
					PathTotalLength - (NextTravelDistance - PathTotalLength);
				PathTravelDirection = -1.0f;
			}
			else
			{
				NextTravelDistance = -NextTravelDistance;
				PathTravelDirection = 1.0f;
			}
			continue;
		}

		if (PathEndBehavior == ECh03WaveEnvironmentPathEndBehavior::Loop)
		{
			NextTravelDistance =
				FMath::Fmod(NextTravelDistance, PathTotalLength);
			if (NextTravelDistance < 0.0f)
			{
				NextTravelDistance += PathTotalLength;
			}
			PathTravelDirection = 1.0f;
			break;
		}

		NextTravelDistance =
			FMath::Clamp(NextTravelDistance, 0.0f, PathTotalLength);
		bHasReachedPathEnd = true;
		break;
	}

	PathTravelDistance =
		FMath::Clamp(NextTravelDistance, 0.0f, PathTotalLength);

	const FVector NewLocation =
		GetPathLocationAtDistance(PathTravelDistance);
	const FVector MovementDelta = NewLocation - PreviousLocation;
	CurrentMovementDirection = MovementDelta.GetSafeNormal2D();
	SetActorLocation(NewLocation);

	UpdateFacingRotation(DeltaSeconds);
}

void ACh03_WaveEnvironmentActor::UpdateFacingRotation(
	const float DeltaSeconds)
{
	if (!bFaceMovementDirection || CurrentMovementDirection.IsNearlyZero())
	{
		return;
	}

	const float DesiredYaw = CurrentMovementDirection.Rotation().Yaw;
	const FRotator DesiredRotation =
		FRotator(0.0f, DesiredYaw, 0.0f) + FacingRotationOffset;

	if (FacingRotationInterpSpeed <= 0.0f)
	{
		SetActorRotation(DesiredRotation);
		return;
	}

	const FRotator NewRotation =
		FMath::RInterpTo(
			GetActorRotation(),
			DesiredRotation,
			DeltaSeconds,
			FacingRotationInterpSpeed);
	SetActorRotation(NewRotation);
}

int32 ACh03_WaveEnvironmentActor::GetPathPointCount() const
{
	const int32 ConfiguredPathPointCount =
		HasValidPathPointActors()
			? GetValidPathPointActorCount()
			: PathPointOffsets.Num();

	return ConfiguredPathPointCount
		+ (bUseInitialLocationAsFirstPathPoint ? 1 : 0);
}

int32 ACh03_WaveEnvironmentActor::GetPathSegmentCount() const
{
	const int32 PathPointCount = GetPathPointCount();
	if (PathPointCount <= 1)
	{
		return 0;
	}

	if (PathEndBehavior == ECh03WaveEnvironmentPathEndBehavior::Loop)
	{
		return PathPointCount;
	}

	return PathPointCount - 1;
}

float ACh03_WaveEnvironmentActor::GetPathTotalLength() const
{
	float TotalLength = 0.0f;
	const int32 PathSegmentCount = GetPathSegmentCount();

	for (int32 SegmentIndex = 0;
		SegmentIndex < PathSegmentCount;
		++SegmentIndex)
	{
		TotalLength += GetPathSegmentLength(SegmentIndex);
	}

	return TotalLength;
}

float ACh03_WaveEnvironmentActor::GetPathSegmentLength(
	const int32 SegmentIndex) const
{
	const FVector SegmentStartLocation =
		GetPathSegmentStartLocation(SegmentIndex);
	const FVector SegmentEndLocation =
		GetPathSegmentEndLocation(SegmentIndex);

	return FVector::Distance(SegmentStartLocation, SegmentEndLocation);
}

FVector ACh03_WaveEnvironmentActor::GetPathPointWorldLocation(
	const int32 PointIndex) const
{
	if (bUseInitialLocationAsFirstPathPoint && PointIndex == 0)
	{
		return InitialActorLocation;
	}

	const int32 ConfiguredPointIndex =
		PointIndex - (bUseInitialLocationAsFirstPathPoint ? 1 : 0);

	if (ConfiguredPointIndex < 0)
	{
		return InitialActorLocation;
	}

	if (HasValidPathPointActors())
	{
		return GetValidPathPointActorLocation(ConfiguredPointIndex);
	}

	return PathPointOffsets.IsValidIndex(ConfiguredPointIndex)
		? InitialActorLocation + PathPointOffsets[ConfiguredPointIndex]
		: InitialActorLocation;
}

FVector ACh03_WaveEnvironmentActor::GetPathSegmentStartLocation(
	const int32 SegmentIndex) const
{
	return GetPathPointWorldLocation(SegmentIndex);
}

FVector ACh03_WaveEnvironmentActor::GetPathSegmentEndLocation(
	const int32 SegmentIndex) const
{
	const int32 PathPointCount = GetPathPointCount();
	if (PathPointCount <= 0)
	{
		return InitialActorLocation;
	}

	const int32 EndPointIndex =
		(SegmentIndex + 1) % PathPointCount;
	return GetPathPointWorldLocation(EndPointIndex);
}

FVector ACh03_WaveEnvironmentActor::GetPathLocationAtDistance(
	const float DistanceAlongPath) const
{
	const int32 PathSegmentCount = GetPathSegmentCount();
	if (PathSegmentCount <= 0)
	{
		return InitialActorLocation;
	}

	float RemainingDistance =
		FMath::Max(0.0f, DistanceAlongPath);
	FVector LastSegmentEndLocation = InitialActorLocation;

	for (int32 SegmentIndex = 0;
		SegmentIndex < PathSegmentCount;
		++SegmentIndex)
	{
		const FVector SegmentStartLocation =
			GetPathSegmentStartLocation(SegmentIndex);
		const FVector SegmentEndLocation =
			GetPathSegmentEndLocation(SegmentIndex);
		const float SegmentLength =
			FVector::Distance(SegmentStartLocation, SegmentEndLocation);

		LastSegmentEndLocation = SegmentEndLocation;

		if (SegmentLength <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		if (RemainingDistance <= SegmentLength)
		{
			const float SegmentAlpha =
				RemainingDistance / SegmentLength;
			return FMath::Lerp(
				SegmentStartLocation,
				SegmentEndLocation,
				SegmentAlpha);
		}

		RemainingDistance -= SegmentLength;
	}

	return LastSegmentEndLocation;
}

bool ACh03_WaveEnvironmentActor::HasValidPathPointActors() const
{
	for (const TObjectPtr<AActor>& PathPointActor : PathPointActors)
	{
		if (IsValid(PathPointActor))
		{
			return true;
		}
	}

	return false;
}

int32 ACh03_WaveEnvironmentActor::GetValidPathPointActorCount() const
{
	int32 ValidPathPointActorCount = 0;

	for (const TObjectPtr<AActor>& PathPointActor : PathPointActors)
	{
		if (IsValid(PathPointActor))
		{
			++ValidPathPointActorCount;
		}
	}

	return ValidPathPointActorCount;
}

FVector ACh03_WaveEnvironmentActor::GetValidPathPointActorLocation(
	const int32 ValidPathPointIndex) const
{
	int32 CurrentValidIndex = 0;

	for (const TObjectPtr<AActor>& PathPointActor : PathPointActors)
	{
		if (!IsValid(PathPointActor))
		{
			continue;
		}

		if (CurrentValidIndex == ValidPathPointIndex)
		{
			return PathPointActor->GetActorLocation();
		}

		++CurrentValidIndex;
	}

	return InitialActorLocation;
}

void ACh03_WaveEnvironmentActor::ResetMovement()
{
	CacheInitialLocationIfNeeded();

	CurrentMovementDirection = FVector::ZeroVector;
	PathTravelDistance = 0.0f;
	PathTravelDirection = 1.0f;
	bHasReachedPathEnd = false;
	SetActorLocationAndRotation(InitialActorLocation, InitialActorRotation);
}
