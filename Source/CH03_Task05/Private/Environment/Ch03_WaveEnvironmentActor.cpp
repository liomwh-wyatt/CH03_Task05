#include "Environment/Ch03_WaveEnvironmentActor.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Ch03_GameStateBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ACh03_WaveEnvironmentActor::ACh03_WaveEnvironmentActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MovingRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MovingRoot"));
	MovingRoot->SetupAttachment(SceneRoot);

	HazardVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("HazardVolume"));
	HazardVolume->SetupAttachment(MovingRoot);
	HazardVolume->SetBoxExtent(FVector(75.0f, 75.0f, 45.0f));
	HazardVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HazardVolume->SetCollisionObjectType(ECC_WorldDynamic);
	HazardVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	HazardVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HazardVolume->SetGenerateOverlapEvents(true);

	StationaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("StationaryMesh"));
	StationaryMesh->SetupAttachment(SceneRoot);
	StationaryMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StationaryMesh->SetVisibility(true);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(MovingRoot);
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

	static ConstructorHelpers::FObjectFinder<USoundBase> ActivationSoundFinder(
		TEXT("/Game/Audio/SFX/Environment/S_RobotVacuum_Warning.S_RobotVacuum_Warning"));
	if (ActivationSoundFinder.Succeeded())
	{
		ActivationFeedback.Sound = ActivationSoundFinder.Object;
		ActivationFeedback.SoundVolumeMultiplier = 0.8f;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> HazardHitSoundFinder(
		TEXT("/Game/Audio/SFX/Environment/S_RobotVacuum_Hit.S_RobotVacuum_Hit"));
	if (HazardHitSoundFinder.Succeeded())
	{
		HazardHitFeedback.Sound = HazardHitSoundFinder.Object;
		HazardHitFeedback.SoundVolumeMultiplier = 0.9f;
	}
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

	if (!bIsEnvironmentActive)
	{
		return;
	}

	if (bMoveWhenActive)
	{
		UpdateActiveMovement(DeltaSeconds);
	}

	if (bRotateMovingPartWhenActive)
	{
		UpdateRotatingPart(DeltaSeconds);
	}

	if (bAffectPlayerOnOverlap)
	{
		ProcessHazardOverlaps();
	}

	if (bDrawMovementPathDebug)
	{
		DrawMovementPathDebug();
	}
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
	if (StationaryMesh)
	{
		StationaryMesh->SetVisibility(bShouldBeVisible, true);
	}

	HazardVolume->SetCollisionEnabled(
		bIsEnvironmentActive
			? ECollisionEnabled::QueryOnly
			: ECollisionEnabled::NoCollision);

	RefreshTickEnabled();

	if (bWasActive == bIsEnvironmentActive)
	{
		return;
	}

	if (bIsEnvironmentActive)
	{
		bPathWarningLogged = false;
		LogPathConfigurationWarningIfNeeded();

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

		if (bResetRotatingPartWhenInactive)
		{
			ResetRotatingPart();
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

bool ACh03_WaveEnvironmentActor::MatchesManagedRuleTag(
	const FName RuleTag) const
{
	return !RuleTag.IsNone() && ActorHasTag(RuleTag);
}

void ACh03_WaveEnvironmentActor::ApplyManagedRule(
	const FCh03_WaveEnvironmentManagedRule& ManagedRule)
{
	ActivateFromWave = FMath::Max(1, ManagedRule.ActivateFromWave);
	DeactivateAfterWave = FMath::Max(0, ManagedRule.DeactivateAfterWave);
	bHideWhenInactive = ManagedRule.bHideWhenInactive;

	if (!ManagedRule.ActiveAnnouncementText.IsEmpty())
	{
		ActiveAnnouncementText = ManagedRule.ActiveAnnouncementText;
	}

	if (ManagedRule.bOverrideHazard)
	{
		bAffectPlayerOnOverlap = ManagedRule.bAffectPlayerOnOverlap;
		ContactEffectInterval =
			FMath::Max(0.05f, ManagedRule.ContactEffectInterval);
		DamageAmount = FMath::Max(0.0f, ManagedRule.DamageAmount);
		bApplySlowOnOverlap = ManagedRule.bApplySlowOnOverlap;
		SlowDuration = FMath::Max(0.1f, ManagedRule.SlowDuration);
		SlowMultiplier =
			FMath::Clamp(ManagedRule.SlowMultiplier, 0.1f, 1.0f);
		bApplyKnockbackOnOverlap =
			ManagedRule.bApplyKnockbackOnOverlap;
		KnockbackStrength =
			FMath::Max(0.0f, ManagedRule.KnockbackStrength);
		KnockbackUpwardStrength =
			FMath::Max(0.0f, ManagedRule.KnockbackUpwardStrength);
		bUseMovementDirectionForKnockback =
			ManagedRule.bUseMovementDirectionForKnockback;
		KnockbackCooldown =
			FMath::Max(0.0f, ManagedRule.KnockbackCooldown);
	}

	if (ManagedRule.bOverridePathMovement)
	{
		bMoveWhenActive = ManagedRule.bMoveWhenActive;
		MovementSpeed = FMath::Max(0.0f, ManagedRule.MovementSpeed);
		bUseInitialLocationAsFirstPathPoint =
			ManagedRule.bUseInitialLocationAsFirstPathPoint;
		PathPointOffsets = ManagedRule.PathPointOffsets;
		PathPointActors.Reset();
		PathEndBehavior = ManagedRule.PathEndBehavior;
		bFaceMovementDirection = ManagedRule.bFaceMovementDirection;
		FacingRotationInterpSpeed =
			FMath::Max(0.0f, ManagedRule.FacingRotationInterpSpeed);
		FacingRotationOffset = ManagedRule.FacingRotationOffset;
		bPathWarningLogged = false;

		if (!bIsEnvironmentActive)
		{
			ResetMovement();
		}
	}

	if (ManagedRule.bOverrideRotatingPart)
	{
		bRotateMovingPartWhenActive =
			ManagedRule.bRotateMovingPartWhenActive;
		RotatingPartAxis = ManagedRule.RotatingPartAxis;
		RotatingPartSpeed = ManagedRule.RotatingPartSpeed;
		bResetRotatingPartWhenInactive =
			ManagedRule.bResetRotatingPartWhenInactive;

		if (!bIsEnvironmentActive && bResetRotatingPartWhenInactive)
		{
			ResetRotatingPart();
		}
	}

	SetEnvironmentActive(IsActiveForWave(LastAppliedWave));
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

	TryApplyHazardEffect(CheonbokCharacter);
}

void ACh03_WaveEnvironmentActor::TryApplyHazardEffect(
	ACh03_CheonbokCharacter* CheonbokCharacter)
{
	if (!bIsEnvironmentActive
		|| !bAffectPlayerOnOverlap
		|| !CheonbokCharacter
		|| CheonbokCharacter->IsDead()
		|| !CanApplyContactEffect())
	{
		return;
	}

	LastHazardEffectTime =
		GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

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

bool ACh03_WaveEnvironmentActor::CanApplyContactEffect() const
{
	if (ContactEffectInterval <= 0.0f)
	{
		return true;
	}

	const float CurrentTime =
		GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	return CurrentTime - LastHazardEffectTime >= ContactEffectInterval;
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
	if (!bHasCachedInitialLocation)
	{
		InitialActorLocation = GetActorLocation();
		InitialActorRotation = GetActorRotation();
		bHasCachedInitialLocation = true;
	}

	if (MovingRoot && !bHasCachedInitialMovingRootRotation)
	{
		InitialMovingRootRelativeRotation = MovingRoot->GetRelativeRotation();
		bHasCachedInitialMovingRootRotation = true;
	}
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

void ACh03_WaveEnvironmentActor::UpdateRotatingPart(
	const float DeltaSeconds)
{
	if (!MovingRoot || FMath::IsNearlyZero(RotatingPartSpeed))
	{
		return;
	}

	const FVector RotationAxis = GetRotatingPartAxisVector();
	if (RotationAxis.IsNearlyZero())
	{
		return;
	}

	const float DeltaRadians =
		FMath::DegreesToRadians(RotatingPartSpeed * DeltaSeconds);
	const FQuat DeltaRotation(RotationAxis, DeltaRadians);
	MovingRoot->AddLocalRotation(
		DeltaRotation,
		false,
		nullptr,
		ETeleportType::None);
}

void ACh03_WaveEnvironmentActor::ProcessHazardOverlaps()
{
	if (!HazardVolume)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	HazardVolume->GetOverlappingActors(
		OverlappingActors,
		ACh03_CheonbokCharacter::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		ACh03_CheonbokCharacter* CheonbokCharacter =
			Cast<ACh03_CheonbokCharacter>(OverlappingActor);
		TryApplyHazardEffect(CheonbokCharacter);
	}
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

void ACh03_WaveEnvironmentActor::DrawMovementPathDebug() const
{
	if (!GetWorld() || !bMoveWhenActive)
	{
		return;
	}

	const int32 PathPointCount = GetPathPointCount();
	const int32 PathSegmentCount = GetPathSegmentCount();
	if (PathPointCount <= 0 || PathSegmentCount <= 0)
	{
		return;
	}

	const FVector DebugOffset(0.0f, 0.0f, MovementPathDebugHeightOffset);

	for (int32 PointIndex = 0; PointIndex < PathPointCount; ++PointIndex)
	{
		DrawDebugSphere(
			GetWorld(),
			GetPathPointWorldLocation(PointIndex) + DebugOffset,
			18.0f,
			8,
			MovementPathDebugColor,
			false,
			0.0f,
			0,
			1.5f);
	}

	for (int32 SegmentIndex = 0; SegmentIndex < PathSegmentCount; ++SegmentIndex)
	{
		DrawDebugLine(
			GetWorld(),
			GetPathSegmentStartLocation(SegmentIndex) + DebugOffset,
			GetPathSegmentEndLocation(SegmentIndex) + DebugOffset,
			MovementPathDebugColor,
			false,
			0.0f,
			0,
			2.0f);
	}
}

void ACh03_WaveEnvironmentActor::LogPathConfigurationWarningIfNeeded()
{
	if (bPathWarningLogged || !bMoveWhenActive)
	{
		return;
	}

	CacheInitialLocationIfNeeded();

	if (GetPathSegmentCount() > 0 && GetPathTotalLength() > KINDA_SMALL_NUMBER)
	{
		return;
	}

	bPathWarningLogged = true;
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s: 이동식 방해물 경로가 유효하지 않습니다. PathPointActors 또는 PathPointOffsets를 확인하세요."),
		*GetName());
}

FVector ACh03_WaveEnvironmentActor::GetRotatingPartAxisVector() const
{
	switch (RotatingPartAxis)
	{
	case ECh03WaveEnvironmentRotationAxis::X:
		return FVector::ForwardVector;
	case ECh03WaveEnvironmentRotationAxis::Y:
		return FVector::RightVector;
	case ECh03WaveEnvironmentRotationAxis::Z:
	default:
		return FVector::UpVector;
	}
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

void ACh03_WaveEnvironmentActor::ResetRotatingPart()
{
	CacheInitialLocationIfNeeded();

	if (!MovingRoot)
	{
		return;
	}

	MovingRoot->SetRelativeRotation(InitialMovingRootRelativeRotation);
}

void ACh03_WaveEnvironmentActor::RefreshTickEnabled()
{
	SetActorTickEnabled(
		bIsEnvironmentActive
		&& (bMoveWhenActive
			|| bRotateMovingPartWhenActive
			|| bAffectPlayerOnOverlap
			|| (bMoveWhenActive && bDrawMovementPathDebug)));
}
