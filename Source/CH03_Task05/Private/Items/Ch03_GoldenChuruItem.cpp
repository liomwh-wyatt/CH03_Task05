#include "Items/Ch03_GoldenChuruItem.h"

ACh03_GoldenChuruItem::ACh03_GoldenChuruItem()
{
	ItemType = TEXT("GoldenChuru");
	ScoreValue = 300;
	RotationSpeed = 140.0f;
	BobAmplitude = 18.0f;
	BobFrequency = 2.0f;
	LifetimeAfterSpawn = 8.0f;
	BlinkStartTime = 2.5f;
	BlinkInterval = 0.12f;
}
