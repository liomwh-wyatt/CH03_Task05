// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_SlowingItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_SlowingItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_SlowingItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Debuff",
		meta = (ClampMin = "0.1", Units = "s"))
	float SlowDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Debuff",
		meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float SpeedMultiplier = 0.5f;
};
