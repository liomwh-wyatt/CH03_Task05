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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Damage",
		meta = (ClampMin = "0.0"))
	float DamageAmount = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Damage")
	TSubclassOf<UDamageType> DamageTypeClass;
};
