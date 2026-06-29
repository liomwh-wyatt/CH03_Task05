// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Environment/Ch03_JumpPadActor.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ACh03_JumpPadActor::ACh03_JumpPadActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	PadMesh->SetupAttachment(SceneRoot);
	PadMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PadMesh->SetCollisionObjectType(ECC_WorldStatic);
	PadMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PadMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 0.2f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PadMesh->SetStaticMesh(CubeMesh.Object);
	}

	LaunchVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("LaunchVolume"));
	LaunchVolume->SetupAttachment(SceneRoot);
	LaunchVolume->SetBoxExtent(FVector(90.0f, 90.0f, 35.0f));
	LaunchVolume->SetRelativeLocation(FVector(0.0f, 0.0f, 35.0f));
	LaunchVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LaunchVolume->SetCollisionObjectType(ECC_WorldDynamic);
	LaunchVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	LaunchVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LaunchVolume->SetGenerateOverlapEvents(true);
}

void ACh03_JumpPadActor::BeginPlay()
{
	Super::BeginPlay();

	if (PadMesh)
	{
		RestingPadMeshRelativeScale = PadMesh->GetRelativeScale3D();
	}

	if (LaunchVolume)
	{
		LaunchVolume->OnComponentBeginOverlap.AddUniqueDynamic(
			this,
			&ACh03_JumpPadActor::HandleLaunchVolumeBeginOverlap);
	}

	SetJumpPadEnabled(bStartEnabled);
}

void ACh03_JumpPadActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateBounceScaleAnimation(DeltaSeconds);
}

void ACh03_JumpPadActor::SetJumpPadEnabled(const bool bNewEnabled)
{
	bIsJumpPadEnabled = bNewEnabled;
	RefreshLaunchVolumeCollision();
}

bool ACh03_JumpPadActor::ActivateJumpPad(
	ACh03_CheonbokCharacter* CheonbokCharacter)
{
	if (!bIsJumpPadEnabled
		|| !IsValid(CheonbokCharacter)
		|| CheonbokCharacter->IsDead()
		|| !CanActivateJumpPad())
	{
		return false;
	}

	LastActivationTime =
		GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	const FVector LaunchVelocity =
		FVector::UpVector * FMath::Max(0.0f, LaunchStrength)
		+ GetActorForwardVector() * FMath::Max(0.0f, ForwardBoostStrength);

	CheonbokCharacter->LaunchCharacter(
		LaunchVelocity,
		bOverrideHorizontalVelocity,
		bOverrideVerticalVelocity);

	StartBounceScaleAnimation();

	UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtActor(
		this,
		ActivationFeedback,
		this);

	OnJumpPadActivated.Broadcast(CheonbokCharacter);
	return true;
}

void ACh03_JumpPadActor::HandleLaunchVolumeBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	(void)OverlappedComponent;
	(void)OtherComp;
	(void)OtherBodyIndex;
	(void)bFromSweep;
	(void)SweepResult;

	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(OtherActor);
	ActivateJumpPad(CheonbokCharacter);
}

bool ACh03_JumpPadActor::CanActivateJumpPad() const
{
	if (ReactivationDelay <= 0.0f)
	{
		return true;
	}

	const float CurrentTime =
		GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	return CurrentTime - LastActivationTime >= ReactivationDelay;
}

void ACh03_JumpPadActor::StartBounceScaleAnimation()
{
	if (!bUseBounceScaleAnimation
		|| !PadMesh
		|| BounceAnimationDuration <= 0.0f)
	{
		return;
	}

	BounceAnimationElapsed = 0.0f;
	bIsBounceAnimationActive = true;
	SetActorTickEnabled(true);
}

void ACh03_JumpPadActor::UpdateBounceScaleAnimation(
	const float DeltaSeconds)
{
	if (!bIsBounceAnimationActive || !PadMesh)
	{
		SetActorTickEnabled(false);
		return;
	}

	BounceAnimationElapsed += FMath::Max(0.0f, DeltaSeconds);

	const float SafeDuration = FMath::Max(KINDA_SMALL_NUMBER, BounceAnimationDuration);
	const float NormalizedTime =
		FMath::Clamp(BounceAnimationElapsed / SafeDuration, 0.0f, 1.0f);

	PadMesh->SetRelativeScale3D(GetBounceScaleAtNormalizedTime(NormalizedTime));

	if (NormalizedTime >= 1.0f)
	{
		PadMesh->SetRelativeScale3D(RestingPadMeshRelativeScale);
		BounceAnimationElapsed = 0.0f;
		bIsBounceAnimationActive = false;
		SetActorTickEnabled(false);
	}
}

FVector ACh03_JumpPadActor::GetBounceScaleAtNormalizedTime(
	const float NormalizedTime) const
{
	const FVector SquashScale =
		RestingPadMeshRelativeScale * SquashScaleMultiplier;
	const FVector StretchScale =
		RestingPadMeshRelativeScale * StretchScaleMultiplier;

	if (NormalizedTime < 0.35f)
	{
		const float Alpha =
			FMath::Clamp(NormalizedTime / 0.35f, 0.0f, 1.0f);
		return FMath::Lerp(
			RestingPadMeshRelativeScale,
			SquashScale,
			FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f));
	}

	if (NormalizedTime < 0.65f)
	{
		const float Alpha =
			FMath::Clamp((NormalizedTime - 0.35f) / 0.3f, 0.0f, 1.0f);
		return FMath::Lerp(
			SquashScale,
			StretchScale,
			FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f));
	}

	const float Alpha =
		FMath::Clamp((NormalizedTime - 0.65f) / 0.35f, 0.0f, 1.0f);
	return FMath::Lerp(
		StretchScale,
		RestingPadMeshRelativeScale,
		FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f));
}

void ACh03_JumpPadActor::RefreshLaunchVolumeCollision() const
{
	if (!LaunchVolume)
	{
		return;
	}

	LaunchVolume->SetCollisionEnabled(
		bIsJumpPadEnabled
			? ECollisionEnabled::QueryOnly
			: ECollisionEnabled::NoCollision);
}
