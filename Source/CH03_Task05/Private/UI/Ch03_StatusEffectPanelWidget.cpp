// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_StatusEffectPanelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/VerticalBox.h"
#include "UI/Ch03_StatusEffectSlotWidget.h"

void UCh03_StatusEffectPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildNativeFallbackWidget();
	ClearStatusEffects();
}

void UCh03_StatusEffectPanelWidget::UpdateStatusEffect(
	const ECheonbokStatusEffect EffectType,
	const FText& Label,
	const bool bIsActive,
	const int32 StackCount,
	const float RemainingTime,
	const float MaxDuration)
{
	if (UCh03_StatusEffectSlotWidget* TargetSlot = FindSlot(EffectType))
	{
		TargetSlot->SetStatusEffect(
			EffectType,
			Label,
			bIsActive,
			StackCount,
			RemainingTime,
			MaxDuration);
	}
}

void UCh03_StatusEffectPanelWidget::ClearStatusEffects()
{
	UpdateStatusEffect(
		ECheonbokStatusEffect::Slow,
		NSLOCTEXT("CheonbokHUD", "SlowStatus", "느려짐"),
		false,
		0,
		0.0f,
		0.0f);

	UpdateStatusEffect(
		ECheonbokStatusEffect::ReverseControl,
		NSLOCTEXT("CheonbokHUD", "ReverseStatus", "조작 반전"),
		false,
		0,
		0.0f,
		0.0f);

	UpdateStatusEffect(
		ECheonbokStatusEffect::MovementLock,
		NSLOCTEXT("CheonbokHUD", "MovementLockStatus", "묶임"),
		false,
		0,
		0.0f,
		0.0f);

	UpdateStatusEffect(
		ECheonbokStatusEffect::DamageShield,
		NSLOCTEXT("CheonbokHUD", "DamageShieldStatus", "방울 보호"),
		false,
		0,
		-1.0f,
		0.0f);
}

void UCh03_StatusEffectPanelWidget::BuildNativeFallbackWidget()
{
	if (!WidgetTree
		|| (SlowSlot
			&& ReverseControlSlot
			&& MovementLockSlot
			&& DamageShieldSlot))
	{
		return;
	}

	UPanelWidget* RootPanel = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		UVerticalBox* RootBox =
			WidgetTree->ConstructWidget<UVerticalBox>(
				UVerticalBox::StaticClass(),
				TEXT("StatusEffectPanelRoot_NativeFallback"));
		WidgetTree->RootWidget = RootBox;
		RootPanel = RootBox;
	}

	auto CreateFallbackSlot =
		[this, RootPanel](
			const FName WidgetName) -> UCh03_StatusEffectSlotWidget*
		{
			UCh03_StatusEffectSlotWidget* NewSlot =
				WidgetTree->ConstructWidget<UCh03_StatusEffectSlotWidget>(
					UCh03_StatusEffectSlotWidget::StaticClass(),
					WidgetName);

			RootPanel->AddChild(NewSlot);
			return NewSlot;
		};

	if (!SlowSlot)
	{
		SlowSlot = CreateFallbackSlot(TEXT("SlowSlot_NativeFallback"));
	}

	if (!ReverseControlSlot)
	{
		ReverseControlSlot =
			CreateFallbackSlot(TEXT("ReverseControlSlot_NativeFallback"));
	}

	if (!MovementLockSlot)
	{
		MovementLockSlot =
			CreateFallbackSlot(TEXT("MovementLockSlot_NativeFallback"));
	}

	if (!DamageShieldSlot)
	{
		DamageShieldSlot =
			CreateFallbackSlot(TEXT("DamageShieldSlot_NativeFallback"));
	}
}

UCh03_StatusEffectSlotWidget* UCh03_StatusEffectPanelWidget::FindSlot(
	const ECheonbokStatusEffect EffectType) const
{
	switch (EffectType)
	{
	case ECheonbokStatusEffect::Slow:
		return SlowSlot;
	case ECheonbokStatusEffect::ReverseControl:
		return ReverseControlSlot;
	case ECheonbokStatusEffect::MovementLock:
		return MovementLockSlot;
	case ECheonbokStatusEffect::DamageShield:
		return DamageShieldSlot;
	default:
		return nullptr;
	}
}
