// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_StatusEffectSlotWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCh03_StatusEffectSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildNativeFallbackWidget();
	RefreshVisuals();
}

void UCh03_StatusEffectSlotWidget::SetStatusEffect(
	const ECheonbokStatusEffect EffectType,
	const FText& Label,
	const bool bIsActive,
	const int32 StackCount,
	const float RemainingTime,
	const float MaxDuration)
{
	CachedEffectType = EffectType;
	CachedLabel = Label;
	bCachedIsActive = bIsActive;
	CachedStackCount = StackCount;
	CachedRemainingTime = RemainingTime;
	CachedMaxDuration = MaxDuration;

	RefreshVisuals();

	OnStatusEffectUpdated(
		CachedEffectType,
		CachedLabel,
		bCachedIsActive,
		CachedStackCount,
		CachedRemainingTime,
		CachedMaxDuration,
		GetRemainingTimePercent());
}

void UCh03_StatusEffectSlotWidget::BuildNativeFallbackWidget()
{
	if (!WidgetTree
		|| (IconImage
			&& LabelText
			&& StackText
			&& RemainingTimeText
			&& RemainingTimeBar))
	{
		return;
	}

	UPanelWidget* RootPanel = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		UHorizontalBox* RootBox = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("StatusEffectSlotRoot_NativeFallback"));
		WidgetTree->RootWidget = RootBox;
		RootPanel = RootBox;
	}

	auto CreateFallbackText =
		[this, RootPanel](const FName WidgetName) -> UTextBlock*
		{
			UTextBlock* NewText =
				WidgetTree->ConstructWidget<UTextBlock>(
					UTextBlock::StaticClass(),
					WidgetName);

			NewText->SetColorAndOpacity(
				FSlateColor(FLinearColor(1.0f, 0.65f, 0.25f, 1.0f)));
			NewText->SetShadowOffset(FVector2D(1.5f, 1.5f));
			NewText->SetShadowColorAndOpacity(
				FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));

			RootPanel->AddChild(NewText);
			return NewText;
		};

	if (!IconImage)
	{
		IconImage = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(),
			TEXT("IconImage_NativeFallback"));
		RootPanel->AddChild(IconImage);
	}

	if (!LabelText)
	{
		LabelText = CreateFallbackText(TEXT("LabelText_NativeFallback"));
	}

	if (!StackText)
	{
		StackText = CreateFallbackText(TEXT("StackText_NativeFallback"));
	}

	if (!RemainingTimeText)
	{
		RemainingTimeText =
			CreateFallbackText(TEXT("RemainingTimeText_NativeFallback"));
	}

	if (!RemainingTimeBar)
	{
		RemainingTimeBar =
			WidgetTree->ConstructWidget<UProgressBar>(
				UProgressBar::StaticClass(),
				TEXT("RemainingTimeBar_NativeFallback"));
		RootPanel->AddChild(RemainingTimeBar);
	}
}

void UCh03_StatusEffectSlotWidget::RefreshVisuals()
{
	SetVisibility(
		bCachedIsActive || !bCollapseWhenInactive
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);

	const FLinearColor TintColor =
		bCachedIsActive ? ActiveTint : InactiveTint;

	if (IconImage)
	{
		IconImage->SetColorAndOpacity(TintColor);
		IconImage->SetBrush(IconBrush);
	}

	if (RemainingTimeBar)
	{
		RemainingTimeBar->SetPercent(
			bCachedIsActive ? GetRemainingTimePercent() : 0.0f);
	}

	if (!bCachedIsActive)
	{
		if (LabelText)
		{
			LabelText->SetText(FText::GetEmpty());
		}

		if (StackText)
		{
			StackText->SetText(FText::GetEmpty());
		}

		if (RemainingTimeText)
		{
			RemainingTimeText->SetText(FText::GetEmpty());
		}

		return;
	}

	if (LabelText)
	{
		LabelText->SetText(CachedLabel);
	}

	if (StackText)
	{
		StackText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"StatusEffectSlotStack",
					"x{0}"),
				FText::AsNumber(FMath::Max(1, CachedStackCount))));
	}

	if (RemainingTimeText)
	{
		RemainingTimeText->SetText(
			CachedRemainingTime >= 0.0f
				? FText::Format(
					NSLOCTEXT(
						"CheonbokHUD",
						"StatusEffectSlotRemainingTime",
						"{0}s"),
					FText::AsNumber(FMath::CeilToInt(CachedRemainingTime)))
				: FText::GetEmpty());
	}
}

float UCh03_StatusEffectSlotWidget::GetRemainingTimePercent() const
{
	if (CachedRemainingTime < 0.0f)
	{
		return bCachedIsActive ? 1.0f : 0.0f;
	}

	if (CachedMaxDuration <= KINDA_SMALL_NUMBER)
	{
		return bCachedIsActive ? 1.0f : 0.0f;
	}

	return FMath::Clamp(
		CachedRemainingTime / CachedMaxDuration,
		0.0f,
		1.0f);
}
