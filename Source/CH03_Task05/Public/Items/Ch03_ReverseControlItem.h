// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_ReverseControlItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_ReverseControlItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_ReverseControlItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|디버프",
		meta = (DisplayName = "방향 반전 지속시간", ClampMin = "0.1", Units = "s"))
	float ReverseControlDuration = 4.0f;
};
