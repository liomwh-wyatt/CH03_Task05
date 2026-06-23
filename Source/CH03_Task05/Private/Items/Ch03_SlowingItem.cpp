// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Items/Ch03_SlowingItem.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Ch03_GameStateBase.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ACh03_SlowingItem::ACh03_SlowingItem()
{
	ItemType = TEXT("SlowingItem");
	RotationSpeed = 90.0f;
	BobAmplitude = 16.0f;
	BobFrequency = 1.8f;
	LifetimeAfterSpawn = 9.0f;
	BlinkStartTime = 2.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
		MeshComponent->SetRelativeScale3D(FVector(0.45f));
	}
}

void ACh03_SlowingItem::ActivateItem_Implementation(
	AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator))
	{
		return;
	}

	if (ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator))
	{
		CheonbokCharacter->ApplySlow(
			SlowDuration,
			SpeedMultiplier);
	}

	if (ACh03_GameStateBase* CheonbokGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr)
	{
		CheonbokGameState->BreakCombo();
	}

	Super::ActivateItem_Implementation(Activator);
}
