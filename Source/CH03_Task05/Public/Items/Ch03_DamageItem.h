#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_DamageItem.generated.h"

class UDamageType;

UCLASS()
class CH03_TASK05_API ACh03_DamageItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_DamageItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|피해",
		meta = (DisplayName = "피해량", ClampMin = "0.0"))
	float DamageAmount = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|피해", meta = (DisplayName = "피해 타입 클래스"))
	TSubclassOf<UDamageType> DamageTypeClass;
};
