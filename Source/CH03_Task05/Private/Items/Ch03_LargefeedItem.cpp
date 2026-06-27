// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Items/Ch03_LargefeedItem.h"

#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ACh03_LargefeedItem::ACh03_LargefeedItem()
{
	ItemType = TEXT("LargeFeed");
	ScoreValue = 100;
	LifetimeAfterSpawn = 14.0f;

	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundFinder(
		TEXT("/Game/Audio/SFX/Items/S_Pickup_LargeFeed.S_Pickup_LargeFeed"));
	if (PickupSoundFinder.Succeeded())
	{
		PickupSound = PickupSoundFinder.Object;
	}
}
