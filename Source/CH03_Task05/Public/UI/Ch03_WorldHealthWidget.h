#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ch03_WorldHealthWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class CH03_TASK05_API UCh03_WorldHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "천복|월드 체력바")
	void SetHealthValues(float CurrentHealth, float MaxHealth);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;

private:
	void CreateFallbackContent();
	void RefreshHealthVisuals();

	float CachedCurrentHealth = 0.0f;
	float CachedMaxHealth = 1.0f;
};
