// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Styling/SlateBrush.h"
#include "Ch03_StatusEffectSlotWidget.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;

UCLASS()
class CH03_TASK05_API UCh03_StatusEffectSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "천복|게임 화면|상태 효과")
	void SetStatusEffect(
		ECheonbokStatusEffect EffectType,
		const FText& Label,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime,
		float MaxDuration);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|게임 화면|상태 효과")
	void OnStatusEffectUpdated(
		ECheonbokStatusEffect EffectType,
		const FText& Label,
		bool bIsActive,
		int32 StackCount,
		float RemainingTime,
		float MaxDuration,
		float RemainingTimePercent);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LabelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StackText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RemainingTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> RemainingTimeBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|게임 화면|상태 효과", meta = (DisplayName = "아이콘 브러시"))
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|게임 화면|상태 효과", meta = (DisplayName = "활성 색상"))
	FLinearColor ActiveTint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|게임 화면|상태 효과", meta = (DisplayName = "비활성 색상"))
	FLinearColor InactiveTint = FLinearColor(1.0f, 1.0f, 1.0f, 0.35f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|게임 화면|상태 효과", meta = (DisplayName = "비활성 시 접기"))
	bool bCollapseWhenInactive = true;

private:
	void BuildNativeFallbackWidget();
	void RefreshVisuals();
	float GetRemainingTimePercent() const;

	ECheonbokStatusEffect CachedEffectType = ECheonbokStatusEffect::Slow;
	FText CachedLabel;
	bool bCachedIsActive = false;
	int32 CachedStackCount = 0;
	float CachedRemainingTime = 0.0f;
	float CachedMaxDuration = 0.0f;
};
