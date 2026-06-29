#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_ShieldBellItem.generated.h"

UCLASS()
class CH03_TASK05_API ACh03_ShieldBellItem : public ACh03_BaseItem
{
	GENERATED_BODY()

public:
	ACh03_ShieldBellItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|보호막",
		meta = (DisplayName = "보호막 수", ClampMin = "1"))
	int32 ShieldStackAmount = 1;
};
