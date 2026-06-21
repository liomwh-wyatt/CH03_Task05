#pragma once

#include "CoreMinimal.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_ScoreItem.generated.h"


UCLASS()
class CH03_TASK05_API ACh03_ScoreItem : public ACh03_BaseItem
{
	GENERATED_BODY()
	
public:
	ACh03_ScoreItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Score",
		meta = (ClampMin = "0"))
	int32 ScoreValue = 50;
};
