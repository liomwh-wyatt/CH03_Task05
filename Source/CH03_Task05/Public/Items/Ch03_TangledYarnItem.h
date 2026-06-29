#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_TangledYarnItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_TangledYarnItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_TangledYarnItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|디버프",
		meta = (DisplayName = "이동 잠금 지속시간", ClampMin = "0.1", Units = "s"))
	float MovementLockDuration = 1.0f;
};
