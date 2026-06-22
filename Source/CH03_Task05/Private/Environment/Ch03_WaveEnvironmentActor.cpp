#include "Environment/Ch03_WaveEnvironmentActor.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
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
		OnEnvironmentActivated(LastAppliedWave, LastAppliedMaxWave);
	}
	else
	{
		if (bResetLocationWhenInactive)
		{
			ResetMovement();
		}

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

	if (DamageAmount > 0.0f)
	{
		UGameplayStatics::ApplyDamage(
			CheonbokCharacter,
			DamageAmount,
			nullptr,
			this,
			nullptr);
	}

	if (bApplySlowOnOverlap)
	{
		CheonbokCharacter->ApplySlow(
			SlowDuration,
			SlowMultiplier);
	}
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
	CacheInitialLocationIfNeeded();

	const float TravelDistance = MovementOffset.Size();
	if (TravelDistance <= KINDA_SMALL_NUMBER || MovementSpeed <= 0.0f)
	{
		return;
	}

	MovementAlpha +=
		(MovementSpeed / TravelDistance)
		* DeltaSeconds
		* MovementDirection;

	if (MovementAlpha >= 1.0f)
	{
		MovementAlpha = 1.0f;
		MovementDirection = -1.0f;
	}
	else if (MovementAlpha <= 0.0f)
	{
		MovementAlpha = 0.0f;
		MovementDirection = 1.0f;
	}

	const FVector TargetLocation =
		InitialActorLocation + MovementOffset;
	const FVector NewLocation =
		FMath::Lerp(InitialActorLocation, TargetLocation, MovementAlpha);

	SetActorLocation(NewLocation);
}

void ACh03_WaveEnvironmentActor::ResetMovement()
{
	CacheInitialLocationIfNeeded();

	MovementAlpha = 0.0f;
	MovementDirection = 1.0f;
	SetActorLocation(InitialActorLocation);
}
