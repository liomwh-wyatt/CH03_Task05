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

	const int32 PathPointCount = GetPathPointCount();
	if (PathPointCount <= 1 || MovementSpeed <= 0.0f || bHasReachedPathEnd)
	{
		CurrentMovementDirection = FVector::ZeroVector;
		return;
	}

	CurrentPathPointIndex =
		FMath::Clamp(CurrentPathPointIndex, 0, PathPointCount - 1);

	const FVector CurrentLocation = GetActorLocation();
	const FVector TargetLocation =
		GetPathPointWorldLocation(CurrentPathPointIndex);
	const FVector ToTarget = TargetLocation - CurrentLocation;
	const float DistanceToTarget = ToTarget.Size();

	if (DistanceToTarget <= PathPointAcceptanceRadius)
	{
		SetActorLocation(TargetLocation);
		CurrentMovementDirection = FVector::ZeroVector;
		AdvancePathTarget();
		return;
	}

	const FVector MoveDirection = ToTarget.GetSafeNormal();
	const float MoveDistance =
		FMath::Min(MovementSpeed * DeltaSeconds, DistanceToTarget);

	if (MoveDistance >= DistanceToTarget - KINDA_SMALL_NUMBER)
	{
		SetActorLocation(TargetLocation);
		CurrentMovementDirection = MoveDirection.GetSafeNormal2D();
		AdvancePathTarget();
		return;
	}

	const FVector NewLocation =
		CurrentLocation + MoveDirection * MoveDistance;

	CurrentMovementDirection = MoveDirection.GetSafeNormal2D();
	SetActorLocation(NewLocation);
}

void ACh03_WaveEnvironmentActor::AdvancePathTarget()
{
	const int32 PathPointCount = GetPathPointCount();

	if (PathPointCount <= 1)
	{
		bHasReachedPathEnd = true;
		CurrentMovementDirection = FVector::ZeroVector;
		return;
	}

	const int32 NextPathPointIndex =
		CurrentPathPointIndex + PathDirection;

	if (NextPathPointIndex >= 0 && NextPathPointIndex < PathPointCount)
	{
		CurrentPathPointIndex = NextPathPointIndex;
		return;
	}

	if (PathEndBehavior == ECh03WaveEnvironmentPathEndBehavior::Reverse)
	{
		PathDirection *= -1;
		CurrentPathPointIndex =
			FMath::Clamp(
				CurrentPathPointIndex + PathDirection,
				0,
				PathPointCount - 1);
		return;
	}

	if (PathEndBehavior == ECh03WaveEnvironmentPathEndBehavior::Loop)
	{
		PathDirection = 1;
		CurrentPathPointIndex = 0;
		return;
	}

	bHasReachedPathEnd = true;
	CurrentMovementDirection = FVector::ZeroVector;
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
	CurrentPathPointIndex = 0;
	PathDirection = 1;
	bHasReachedPathEnd = false;
	SetActorLocation(InitialActorLocation);
}
