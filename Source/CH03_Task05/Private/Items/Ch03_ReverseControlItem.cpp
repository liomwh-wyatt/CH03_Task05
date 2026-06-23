// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Items/Ch03_ReverseControlItem.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Ch03_GameStateBase.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ACh03_ReverseControlItem::ACh03_ReverseControlItem()
{
	ItemType = TEXT("ReverseControlItem");
	RotationSpeed = 120.0f;
	BobAmplitude = 18.0f;
	BobFrequency = 2.0f;
	LifetimeAfterSpawn = 9.0f;
	BlinkStartTime = 2.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
		MeshComponent->SetRelativeScale3D(FVector(0.5f));
	}
}

void ACh03_ReverseControlItem::ActivateItem_Implementation(
	AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator))
	{
		return;
	}

	if (ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator))
	{
		CheonbokCharacter->ApplyReverseControl(
			ReverseControlDuration);
	}

	if (ACh03_GameStateBase* CheonbokGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr)
	{
		CheonbokGameState->BreakCombo();
	}

	Super::ActivateItem_Implementation(Activator);
}
