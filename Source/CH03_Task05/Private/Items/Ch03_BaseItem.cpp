#include "Items/Ch03_BaseItem.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ACh03_BaseItem::ACh03_BaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(
		TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(SceneRoot);
	CollisionComponent->SetSphereRadius(75.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemType = TEXT("BaseItem");
}

void ACh03_BaseItem::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(
		this,
		&ACh03_BaseItem::HandleBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(
		this,
		&ACh03_BaseItem::HandleEndOverlap);
}

void ACh03_BaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsConsumed)
	{
		return;
	}

	RunningTime += DeltaTime;
	UpdateLifetime(DeltaTime);

	if (bIsConsumed)
	{
		return;
	}

	UpdateItemMovement(DeltaTime);
}

void ACh03_BaseItem::UpdateItemMovement(const float DeltaTime)
{
	AddActorLocalRotation(
		FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));

	if (BobAmplitude > 0.0f && BobFrequency > 0.0f)
	{
		FVector NewLocation = InitialLocation;
		NewLocation.Z += FMath::Sin(
			RunningTime * BobFrequency * 2.0f * PI) * BobAmplitude;
		SetActorLocation(NewLocation);
	}
}

void ACh03_BaseItem::UpdateLifetime(const float DeltaTime)
{
	if (!bExpireAfterSpawn || LifetimeAfterSpawn <= 0.0f)
	{
		return;
	}

	SpawnLifeElapsed += FMath::Max(0.0f, DeltaTime);
	const float RemainingTime = LifetimeAfterSpawn - SpawnLifeElapsed;

	if (bBlinkBeforeExpire
		&& BlinkStartTime > 0.0f
		&& RemainingTime <= BlinkStartTime
		&& MeshComponent)
	{
		BlinkElapsed += FMath::Max(0.0f, DeltaTime);
		if (BlinkElapsed >= BlinkInterval)
		{
			BlinkElapsed = 0.0f;
			bIsBlinkVisible = !bIsBlinkVisible;
			MeshComponent->SetVisibility(bIsBlinkVisible, true);
		}
	}

	if (RemainingTime <= 0.0f)
	{
		ExpireItem();
	}
}

void ACh03_BaseItem::ExpireItem()
{
	if (bIsConsumed)
	{
		return;
	}

	bIsConsumed = true;

	if (CollisionComponent)
	{
		CollisionComponent->SetGenerateOverlapEvents(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (MeshComponent)
	{
		MeshComponent->SetVisibility(false, true);
	}

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("%s expired after %.1f seconds."),
		*ItemType.ToString(),
		SpawnLifeElapsed);

	Destroy();
}

void ACh03_BaseItem::OnItemOverlap_Implementation(AActor* OverlapActor)
{
	if (!bIsConsumed && CanBeActivatedBy(OverlapActor))
	{
		ICh03_ItemInterface::Execute_ActivateItem(this, OverlapActor);
	}
}

void ACh03_BaseItem::OnItemEndOverlap_Implementation(AActor* OverlapActor)
{
	(void)OverlapActor;
}

void ACh03_BaseItem::ActivateItem_Implementation(AActor* Activator)
{
	if (bIsConsumed || !CanBeActivatedBy(Activator))
	{
		return;
	}

	bIsConsumed = true;
	CollisionComponent->SetGenerateOverlapEvents(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false, true);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation());
	}

	OnCollected(Activator);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s collected item %s."),
		*GetNameSafe(Activator),
		*ItemType.ToString());

	if (DestroyDelay <= 0.0f)
	{
		Destroy();
	}
	else
	{
		SetLifeSpan(DestroyDelay);
	}
}

FName ACh03_BaseItem::GetItemType_Implementation() const
{
	return ItemType;
}

void ACh03_BaseItem::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	(void)OverlappedComponent;
	(void)OtherComponent;
	(void)OtherBodyIndex;
	(void)bFromSweep;
	(void)SweepResult;

	ICh03_ItemInterface::Execute_OnItemOverlap(this, OtherActor);
}

void ACh03_BaseItem::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
{
	(void)OverlappedComponent;
	(void)OtherComponent;
	(void)OtherBodyIndex;

	ICh03_ItemInterface::Execute_OnItemEndOverlap(this, OtherActor);
}

bool ACh03_BaseItem::CanBeActivatedBy(const AActor* Activator) const
{
	return IsValid(Activator)
		&& Activator->IsA<ACh03_CheonbokCharacter>();
}
