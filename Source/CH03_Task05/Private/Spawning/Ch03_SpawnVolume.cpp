// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Spawning/Ch03_SpawnVolume.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Items/Ch03_BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ACh03_SpawnVolume::ACh03_SpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBounds"));
	SetRootComponent(SpawnBounds);
	SpawnBounds->SetBoxExtent(FVector(1200.0f, 1000.0f, 300.0f));
	SpawnBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnBounds->SetHiddenInGame(true);
}

void ACh03_SpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		SpawnInitialItems();
		StartSpawning();
	}
}

void ACh03_SpawnVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawning();
	SpawnedItems.Reset();

	Super::EndPlay(EndPlayReason);
}

void ACh03_SpawnVolume::ApplyWaveSettings(
	const TArray<FCh03_SpawnItemEntry>& NewSpawnEntries,
	const int32 NewInitialSpawnCount,
	const int32 NewMaxAliveItems,
	const float NewSpawnInterval)
{
	StopSpawning();
	ClearSpawnedItems();

	const bool bHasNewValidItemEntry =
		NewSpawnEntries.ContainsByPredicate(
			[](const FCh03_SpawnItemEntry& Entry)
			{
				return Entry.ItemClass && Entry.Weight > 0.0f;
			});

	if (bHasNewValidItemEntry)
	{
		SpawnEntries = NewSpawnEntries;
	}

	MaxAliveItems = FMath::Max(1, NewMaxAliveItems);
	InitialSpawnCount = FMath::Clamp(
		NewInitialSpawnCount,
		0,
		MaxAliveItems);
	SpawnInterval = FMath::Max(0.1f, NewSpawnInterval);

	const bool bHasValidItemEntry =
		SpawnEntries.ContainsByPredicate(
		[](const FCh03_SpawnItemEntry& Entry)
		{
			return Entry.ItemClass && Entry.Weight > 0.0f;
		});

	if (!bHasValidItemEntry)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("%s: Applied wave settings have no valid item entry."),
			*GetName());
	}
	else if (!bHasNewValidItemEntry)
	{
		UE_LOG(
			LogTemp,
			Verbose,
			TEXT("%s: Wave item entries are empty. Keeping existing item entries."),
			*GetName());
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s: Wave settings applied. Initial=%d, MaxAlive=%d, Interval=%.2f"),
		*GetName(),
		InitialSpawnCount,
		MaxAliveItems,
		SpawnInterval);
}

void ACh03_SpawnVolume::SpawnInitialItems()
{
	SpawnItems(InitialSpawnCount);
}

void ACh03_SpawnVolume::StartSpawning()
{
	if (!GetWorld() || SpawnInterval <= 0.0f)
	{
		return;
	}

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ACh03_SpawnVolume::MaintainSpawnPopulation,
		SpawnInterval,
		true);
}

void ACh03_SpawnVolume::StopSpawning()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void ACh03_SpawnVolume::SpawnItems(const int32 Count)
{
	if (Count <= 0)
	{
		return;
	}

	PruneInvalidItems();

	for (int32 Index = 0; Index < Count && SpawnedItems.Num() < MaxAliveItems; ++Index)
	{
		SpawnOneItem();
	}
}

ACh03_BaseItem* ACh03_SpawnVolume::SpawnOneItem()
{
	PruneInvalidItems();

	if (!GetWorld() || SpawnedItems.Num() >= MaxAliveItems)
	{
		return nullptr;
	}

	const TSubclassOf<ACh03_BaseItem> SelectedItemClass = SelectWeightedItemClass();
	if (!SelectedItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: SpawnEntries has no valid item class."), *GetName());
		return nullptr;
	}

	return SpawnItemOfClass(SelectedItemClass, false);
}

ACh03_BaseItem* ACh03_SpawnVolume::SpawnItemOfClass(
	const TSubclassOf<ACh03_BaseItem> ItemClass,
	const bool bAllowExceedMaxAliveItems)
{
	PruneInvalidItems();

	if (!GetWorld() || !ItemClass)
	{
		return nullptr;
	}

	if (!bAllowExceedMaxAliveItems && SpawnedItems.Num() >= MaxAliveItems)
	{
		return nullptr;
	}

	FVector SpawnLocation;
	if (!FindValidSpawnLocation(SpawnLocation))
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Failed to find a safe spawn location."), *GetName());
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	const FRotator SpawnRotation(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);
	ACh03_BaseItem* SpawnedItem = GetWorld()->SpawnActor<ACh03_BaseItem>(
		ItemClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParameters);

	if (IsValid(SpawnedItem))
	{
		SpawnedItems.Add(SpawnedItem);
		SpawnedItem->OnDestroyed.AddDynamic(this, &ACh03_SpawnVolume::HandleSpawnedItemDestroyed);
	}

	return SpawnedItem;
}

void ACh03_SpawnVolume::ClearSpawnedItems()
{
	const TArray<TObjectPtr<ACh03_BaseItem>> ItemsToDestroy = SpawnedItems;
	SpawnedItems.Reset();

	for (ACh03_BaseItem* Item : ItemsToDestroy)
	{
		if (IsValid(Item))
		{
			Item->OnDestroyed.RemoveDynamic(this, &ACh03_SpawnVolume::HandleSpawnedItemDestroyed);
			Item->Destroy();
		}
	}
}

void ACh03_SpawnVolume::MaintainSpawnPopulation()
{
	PruneInvalidItems();

	if (SpawnedItems.Num() < MaxAliveItems)
	{
		SpawnOneItem();
	}
}

void ACh03_SpawnVolume::PruneInvalidItems()
{
	SpawnedItems.RemoveAll(
		[](const TObjectPtr<ACh03_BaseItem>& Item)
		{
			return !IsValid(Item);
		});
}

bool ACh03_SpawnVolume::FindValidSpawnLocation(FVector& OutLocation) const
{
	if (!GetWorld() || !SpawnBounds)
	{
		return false;
	}

	const FVector BoxExtent = SpawnBounds->GetUnscaledBoxExtent();
	const FTransform BoundsTransform = SpawnBounds->GetComponentTransform();

	for (int32 Attempt = 0; Attempt < MaxLocationAttempts; ++Attempt)
	{
		const FVector LocalTraceOrigin(
			FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
			FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
			BoxExtent.Z);

		const FVector WorldTraceOrigin = BoundsTransform.TransformPosition(LocalTraceOrigin);
		const FVector TraceStart = WorldTraceOrigin + FVector::UpVector * GroundTraceHeight;
		const FVector TraceEnd = WorldTraceOrigin - FVector::UpVector * GroundTraceDepth;

		FHitResult GroundHit;
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(Ch03SpawnGroundTrace), false, this);
		const bool bHitGround = GetWorld()->LineTraceSingleByChannel(
			GroundHit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			TraceParams);

		if (!bHitGround || !GroundHit.bBlockingHit)
		{
			if (bDrawDebug)
			{
				DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, SpawnInterval);
			}
			continue;
		}

		const FVector CandidateLocation =
			GroundHit.ImpactPoint + FVector::UpVector * (SpawnClearanceRadius + 5.0f);
		const bool bSafeLocation = IsLocationSafe(CandidateLocation);

		if (bDrawDebug)
		{
			DrawDebugLine(
				GetWorld(),
				TraceStart,
				GroundHit.ImpactPoint,
				bSafeLocation ? FColor::Green : FColor::Yellow,
				false,
				SpawnInterval);
			DrawDebugSphere(
				GetWorld(),
				CandidateLocation,
				SpawnClearanceRadius,
				12,
				bSafeLocation ? FColor::Green : FColor::Red,
				false,
				SpawnInterval);
		}

		if (bSafeLocation)
		{
			OutLocation = CandidateLocation;
			return true;
		}
	}

	return false;
}

bool ACh03_SpawnVolume::IsLocationSafe(const FVector& Location) const
{
	if (!GetWorld())
	{
		return false;
	}

	if (const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (FVector::DistSquared2D(PlayerPawn->GetActorLocation(), Location)
			< FMath::Square(PlayerExclusionRadius))
		{
			return false;
		}
	}

	for (const ACh03_BaseItem* SpawnedItem : SpawnedItems)
	{
		if (IsValid(SpawnedItem)
			&& FVector::DistSquared2D(SpawnedItem->GetActorLocation(), Location)
			< FMath::Square(MinimumItemSpacing))
		{
			return false;
		}
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(Ch03SpawnClearance), false, this);
	const FCollisionShape ClearanceShape = FCollisionShape::MakeSphere(SpawnClearanceRadius);

	return !GetWorld()->OverlapAnyTestByObjectType(
		Location,
		FQuat::Identity,
		ObjectQueryParams,
		ClearanceShape,
		CollisionParams);
}

TSubclassOf<ACh03_BaseItem> ACh03_SpawnVolume::SelectWeightedItemClass() const
{
	float TotalWeight = 0.0f;

	for (const FCh03_SpawnItemEntry& Entry : SpawnEntries)
	{
		if (Entry.ItemClass && Entry.Weight > 0.0f)
		{
			TotalWeight += Entry.Weight;
		}
	}

	if (TotalWeight <= 0.0f)
	{
		return nullptr;
	}

	float SelectionValue = FMath::FRandRange(0.0f, TotalWeight);
	for (const FCh03_SpawnItemEntry& Entry : SpawnEntries)
	{
		if (!Entry.ItemClass || Entry.Weight <= 0.0f)
		{
			continue;
		}

		SelectionValue -= Entry.Weight;
		if (SelectionValue <= 0.0f)
		{
			return Entry.ItemClass;
		}
	}

	for (int32 Index = SpawnEntries.Num() - 1; Index >= 0; --Index)
	{
		if (SpawnEntries[Index].ItemClass && SpawnEntries[Index].Weight > 0.0f)
		{
			return SpawnEntries[Index].ItemClass;
		}
	}

	return nullptr;
}

void ACh03_SpawnVolume::HandleSpawnedItemDestroyed(AActor* DestroyedActor)
{
	SpawnedItems.Remove(Cast<ACh03_BaseItem>(DestroyedActor));
}
