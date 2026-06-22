#include "UI/Ch03_WorldHealthWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCh03_WorldHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateFallbackContent();
	RefreshHealthVisuals();
}

void UCh03_WorldHealthWidget::SetHealthValues(
	const float CurrentHealth,
	const float MaxHealth)
{
	CachedCurrentHealth = FMath::Max(0.0f, CurrentHealth);
	CachedMaxHealth = FMath::Max(1.0f, MaxHealth);

	RefreshHealthVisuals();
}

void UCh03_WorldHealthWidget::CreateFallbackContent()
{
	if (!WidgetTree || (HealthBar && HealthText))
	{
		return;
	}

	UCanvasPanel* RootCanvas =
		Cast<UCanvasPanel>(WidgetTree->RootWidget);
	if (!RootCanvas)
	{
		RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("WorldHealthRoot_NativeFallback"));
		WidgetTree->RootWidget = RootCanvas;
	}

	if (!HealthBar)
	{
		HealthBar = WidgetTree->ConstructWidget<UProgressBar>(
			UProgressBar::StaticClass(),
			TEXT("HealthBar_NativeFallback"));
		HealthBar->SetFillColorAndOpacity(
			FLinearColor(0.9f, 0.25f, 0.35f, 1.0f));

		UCanvasPanelSlot* BarSlot =
			RootCanvas->AddChildToCanvas(HealthBar);
		BarSlot->SetAnchors(FAnchors(0.0f, 0.0f));
		BarSlot->SetAlignment(FVector2D(0.0f, 0.0f));
		BarSlot->SetPosition(FVector2D(0.0f, 0.0f));
		BarSlot->SetSize(FVector2D(220.0f, 22.0f));
	}

	if (!HealthText)
	{
		HealthText = WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			TEXT("HealthText_NativeFallback"));
		HealthText->SetJustification(ETextJustify::Center);
		HealthText->SetColorAndOpacity(
			FSlateColor(FLinearColor::White));
		HealthText->SetShadowOffset(FVector2D(1.0f, 1.0f));
		HealthText->SetShadowColorAndOpacity(
			FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));

		FSlateFontInfo FontInfo = HealthText->GetFont();
		FontInfo.Size = 14;
		HealthText->SetFont(FontInfo);

		UCanvasPanelSlot* TextSlot =
			RootCanvas->AddChildToCanvas(HealthText);
		TextSlot->SetAnchors(FAnchors(0.0f, 0.0f));
		TextSlot->SetAlignment(FVector2D(0.0f, 0.0f));
		TextSlot->SetPosition(FVector2D(0.0f, 24.0f));
		TextSlot->SetSize(FVector2D(220.0f, 24.0f));
	}
}

void UCh03_WorldHealthWidget::RefreshHealthVisuals()
{
	const float HealthPercent = CachedMaxHealth > 0.0f
		? FMath::Clamp(CachedCurrentHealth / CachedMaxHealth, 0.0f, 1.0f)
		: 0.0f;

	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}

	if (HealthText)
	{
		HealthText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokWorldHealth",
					"HealthFormat",
					"HP {0}%"),
				FText::AsNumber(FMath::RoundToInt(HealthPercent * 100.0f))));
	}
}
