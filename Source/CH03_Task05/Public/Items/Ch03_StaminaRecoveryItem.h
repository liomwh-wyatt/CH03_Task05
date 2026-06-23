#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_StaminaRecoveryItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_StaminaRecoveryItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_StaminaRecoveryItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stamina",
		meta = (ClampMin = "0.0"))
	float StaminaRecoveryAmount = 35.0f;
};
