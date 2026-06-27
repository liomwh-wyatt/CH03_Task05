// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Items/Ch03_SmallfeedItem.h"

#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ACh03_SmallfeedItem::ACh03_SmallfeedItem()
{
	ItemType = TEXT("SmallFeed");
	ScoreValue = 50;

	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundFinder(
		TEXT("/Game/Audio/SFX/Items/S_Pickup_SmallFeed.S_Pickup_SmallFeed"));
	if (PickupSoundFinder.Succeeded())
	{
		PickupSound = PickupSoundFinder.Object;
	}
}
