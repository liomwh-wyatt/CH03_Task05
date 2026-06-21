#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Ch03_ItemInterface.generated.h"

UINTERFACE(BlueprintType)
class UCh03_ItemInterface : public UInterface
{
	GENERATED_BODY()
};

class CH03_TASK05_API ICh03_ItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void OnItemOverlap(AActor* OverlapActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void OnItemEndOverlap(AActor* OverlapActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void ActivateItem(AActor* Activator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	FName GetItemType() const;
};
