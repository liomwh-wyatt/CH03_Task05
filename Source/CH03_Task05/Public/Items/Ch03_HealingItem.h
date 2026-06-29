#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_HealingItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_HealingItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_HealingItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|체력 회복",
		meta = (DisplayName = "체력 회복량", ClampMin = "0.0"))
	float HealingAmount = 20.0f;
};
