// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Ch03_StatusEffectSlotWidget.generated.h"

class UTextBlock;

UCLASS()
class CH03_TASK05_API UCh03_StatusEffectSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cheonbok|HUD|Status Effect")
	void SetStatusEffect(
		ECheonbokStatusEffect EffectType,
		const FText& Label,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Cheonbok|HUD|Status Effect")
	void OnStatusEffectUpdated(
		ECheonbokStatusEffect EffectType,
		const FText& Label,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LabelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StackText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RemainingTimeText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|HUD|Status Effect")
	bool bCollapseWhenInactive = true;

private:
	void BuildNativeFallbackWidget();
	void RefreshVisuals();

	ECheonbokStatusEffect CachedEffectType = ECheonbokStatusEffect::Slow;
	FText CachedLabel;
	bool bCachedIsActive = false;
	int32 CachedStackCount = 0;
	float CachedRemainingTime = 0.0f;
};
