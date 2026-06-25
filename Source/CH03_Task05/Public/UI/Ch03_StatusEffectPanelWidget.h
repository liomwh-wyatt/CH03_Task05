// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Ch03_StatusEffectPanelWidget.generated.h"

class UCh03_StatusEffectSlotWidget;

UCLASS()
class CH03_TASK05_API UCh03_StatusEffectPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cheonbok|HUD|Status Effect")
	void UpdateStatusEffect(
		ECheonbokStatusEffect EffectType,
		const FText& Label,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime,
		float MaxDuration);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|HUD|Status Effect")
	void ClearStatusEffects();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCh03_StatusEffectSlotWidget> SlowSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCh03_StatusEffectSlotWidget> ReverseControlSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCh03_StatusEffectSlotWidget> MovementLockSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCh03_StatusEffectSlotWidget> DamageShieldSlot;

private:
	void BuildNativeFallbackWidget();
	UCh03_StatusEffectSlotWidget* FindSlot(ECheonbokStatusEffect EffectType) const;
};
