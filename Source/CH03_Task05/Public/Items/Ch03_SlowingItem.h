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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|디버프",
		meta = (DisplayName = "감속 지속시간", ClampMin = "0.1", Units = "s"))
	float SlowDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|디버프",
		meta = (DisplayName = "속도 배율", ClampMin = "0.1", ClampMax = "1.0"))
	float SpeedMultiplier = 0.5f;
};
