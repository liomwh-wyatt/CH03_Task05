// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/Ch03_GameStateBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UI/Ch03_StatusEffectPanelWidget.h"

void UCh03_GameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateWaveBannerTextFallback();
	CreateStaminaFallbacks();
	CreateComboFallbacks();
	CreateStatusEffectTextFallbacks();
	InitializeTransientTextWidgets();
	BindToGameState();
	BindToCharacter();
}

void UCh03_GameHUDWidget::NativeDestruct()
{
	UnbindFromGameState();
	UnbindFromCharacter();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			CharacterBindRetryTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(
			StatusEffectRefreshTimerHandle);
	}

	Super::NativeDestruct();
}

void UCh03_GameHUDWidget::NativeTick(
	const FGeometry& MyGeometry,
	const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateWaveBannerAnimation(InDeltaTime);
	UpdateComboRewardFeedback(InDeltaTime);
	UpdateTimerBarDisplay();
}

void UCh03_GameHUDWidget::HandleScoreChanged(int32 NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"ScoreFormat",
					"🫓:{0}"),
				FText::AsNumber(NewScore)));
	}

	OnScoreUpdated(NewScore);
}

void UCh03_GameHUDWidget::HandleHealthChanged(
	float CurrentHealth,
	float MaxHealth)
{
	const float HealthPercent = MaxHealth > 0.0f
		? FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f)
		: 0.0f;

	if (HealthText)
	{
		HealthText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"HealthFormat",
					"HP: {0} / {1}"),
				FText::AsNumber(FMath::RoundToInt(CurrentHealth)),
				FText::AsNumber(FMath::RoundToInt(MaxHealth))));
	}

	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}

	OnHealthUpdated(CurrentHealth, MaxHealth, HealthPercent);
}

void UCh03_GameHUDWidget::HandleStaminaChanged(
	const float CurrentStamina,
	const float MaxStamina)
{
	const float StaminaPercent = MaxStamina > 0.0f
		? FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f)
		: 0.0f;

	if (StaminaText)
	{
		StaminaText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"StaminaFormat",
					"STA: {0} / {1}"),
				FText::AsNumber(FMath::RoundToInt(CurrentStamina)),
				FText::AsNumber(FMath::RoundToInt(MaxStamina))));
	}

	if (StaminaBar)
	{
		StaminaBar->SetPercent(StaminaPercent);
	}

	OnStaminaUpdated(CurrentStamina, MaxStamina, StaminaPercent);
}

void UCh03_GameHUDWidget::HandleWaveChanged(
	const int32 CurrentWave,
	const int32 MaxWave)
{
	if (LastObservedTimerWave != CurrentWave)
	{
		LastObservedTimerWave = CurrentWave;
		TimerBarMaxSeconds = 0;
		ResetTimerBar();
	}

	if (WaveText)
	{
		if (CurrentWave <= 0)
		{
			WaveText->SetText(
				NSLOCTEXT(
					"CheonbokHUD",
					"WaveReady",
					"Ready"));
		}
		else
		{
			WaveText->SetText(
				FText::Format(
					NSLOCTEXT(
						"CheonbokHUD",
						"WaveFormat",
						"Wave {0} / {1}"),
					FText::AsNumber(CurrentWave),
					FText::AsNumber(MaxWave)));
		}
	}

	if (CurrentWave <= 0)
	{
		UpdateTimerBarPercent(0);
	}

	OnWaveUpdated(CurrentWave, MaxWave);
}

void UCh03_GameHUDWidget::HandleRemainingTimeChanged(
	const int32 NewRemainingTime)
{
	const int32 SafeRemainingTime = FMath::Max(
		0,
		NewRemainingTime);
	const int32 Minutes = SafeRemainingTime / 60;
	const int32 Seconds = SafeRemainingTime % 60;

	if (TimerText)
	{
		TimerText->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("%02d:%02d"),
					Minutes,
					Seconds)));
	}

	UpdateTimerBarPercent(SafeRemainingTime);
	OnRemainingTimeUpdated(SafeRemainingTime);
}

void UCh03_GameHUDWidget::UpdateTimerBarPercent(
	const int32 SafeRemainingTime)
{
	if (!TimerBar)
	{
		return;
	}

	if (SafeRemainingTime > TimerBarMaxSeconds)
	{
		TimerBarMaxSeconds = SafeRemainingTime;
	}

	const int32 MaxSeconds =
		BoundGameState && BoundGameState->GetWaveDuration() > 0
			? BoundGameState->GetWaveDuration()
			: TimerBarMaxSeconds;

	const float TimerPercent = MaxSeconds > 0
		? FMath::Clamp(
			static_cast<float>(SafeRemainingTime)
			/ static_cast<float>(MaxSeconds),
			0.0f,
			1.0f)
		: 0.0f;

	if (MaxSeconds <= 0 || SafeRemainingTime <= 0)
	{
		ResetTimerBar();
		return;
	}

	TimerBarDurationSeconds = static_cast<float>(MaxSeconds);
	TimerBarEndTimeSeconds =
		(GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f)
		+ static_cast<float>(SafeRemainingTime);
	bIsTimerBarCountingDown = true;
	TimerBar->SetPercent(TimerPercent);
}

void UCh03_GameHUDWidget::UpdateTimerBarDisplay()
{
	if (!TimerBar || !bIsTimerBarCountingDown)
	{
		return;
	}

	const float CurrentWorldTime =
		GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	const float RemainingSeconds = FMath::Max(
		0.0f,
		TimerBarEndTimeSeconds - CurrentWorldTime);
	const float TimerPercent = TimerBarDurationSeconds > 0.0f
		? FMath::Clamp(
			RemainingSeconds / TimerBarDurationSeconds,
			0.0f,
			1.0f)
		: 0.0f;

	TimerBar->SetPercent(TimerPercent);

	if (RemainingSeconds <= 0.0f)
	{
		bIsTimerBarCountingDown = false;
	}
}

void UCh03_GameHUDWidget::ResetTimerBar()
{
	TimerBarDurationSeconds = 0.0f;
	TimerBarEndTimeSeconds = 0.0f;
	bIsTimerBarCountingDown = false;

	if (TimerBar)
	{
		TimerBar->SetPercent(0.0f);
	}
}

void UCh03_GameHUDWidget::InitializeTransientTextWidgets()
{
	auto ClearAndCollapseText =
		[](UTextBlock* TargetText)
		{
			if (!TargetText)
			{
				return;
			}

			TargetText->SetText(FText::GetEmpty());
			TargetText->SetVisibility(ESlateVisibility::Collapsed);
			TargetText->SetRenderOpacity(1.0f);
		};

	ClearAndCollapseText(WaveBannerText);
	ClearAndCollapseText(ComboText);
	ClearAndCollapseText(ComboRewardText);
	ClearAndCollapseText(SlowStatusText);
	ClearAndCollapseText(ReverseControlStatusText);
	ClearAndCollapseText(MovementLockStatusText);
	ClearAndCollapseText(DamageShieldStatusText);
}

void UCh03_GameHUDWidget::HandleAnnouncementChanged(
	const FText NewAnnouncement)
{
	const bool bIsVisible = !NewAnnouncement.IsEmpty();

	if (WaveBannerText)
	{
		if (bIsVisible)
		{
			WaveBannerText->SetText(NewAnnouncement);
			WaveBannerText->SetVisibility(
				ESlateVisibility::HitTestInvisible);
			PlayWaveBannerIntroAnimation();
		}
		else
		{
			PlayWaveBannerOutroAnimation();
		}
	}

	OnAnnouncementUpdated(NewAnnouncement, bIsVisible);
}

void UCh03_GameHUDWidget::HandleComboChanged(
	const int32 ComboCount,
	const float ComboTimeRemaining,
	const float ScoreMultiplier,
	const bool bIsActive)
{
	const bool bShouldShowCombo = bIsActive && ComboCount >= 2;

	if (ComboText)
	{
		ComboText->SetVisibility(
			bShouldShowCombo
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed);

		if (bShouldShowCombo)
		{
			ComboText->SetText(
				FText::FromString(
					FString::Printf(
						TEXT("우다다 x%d  %.2fx  %.1fs"),
						ComboCount,
						ScoreMultiplier,
						FMath::Max(0.0f, ComboTimeRemaining))));
		}
		else
		{
			ComboText->SetText(FText::GetEmpty());
		}
	}

	OnComboUpdated(
		ComboCount,
		ComboTimeRemaining,
		ScoreMultiplier,
		bShouldShowCombo);
}

void UCh03_GameHUDWidget::HandleComboRewardTriggered(
	const int32 ComboCount,
	const FText RewardText)
{
	if (ComboRewardText)
	{
		ComboRewardText->SetColorAndOpacity(
			FSlateColor(FLinearColor(1.0f, 0.55f, 0.12f, 1.0f)));
		ComboRewardText->SetText(RewardText);
		ComboRewardText->SetVisibility(ESlateVisibility::HitTestInvisible);
		ComboRewardText->SetRenderOpacity(1.0f);
		ComboRewardRemainingTime = ComboRewardDisplayDuration;
	}

	OnComboRewardUpdated(ComboCount, RewardText);
}

void UCh03_GameHUDWidget::HandleComboBroken(
	const int32 PreviousComboCount,
	const ECh03ComboBreakReason BreakReason)
{
	if (ComboRewardText)
	{
		ComboRewardText->SetColorAndOpacity(
			FSlateColor(FLinearColor(1.0f, 0.22f, 0.14f, 1.0f)));
		ComboRewardText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"ComboBroken",
					"우다다 끊김 x{0}"),
				FText::AsNumber(FMath::Max(0, PreviousComboCount))));
		ComboRewardText->SetVisibility(ESlateVisibility::HitTestInvisible);
		ComboRewardText->SetRenderOpacity(1.0f);
		ComboRewardRemainingTime = ComboBreakDisplayDuration;
	}

	OnComboBroken(PreviousComboCount, BreakReason);
}

void UCh03_GameHUDWidget::HandleStatusEffectChanged(
	const ECheonbokStatusEffect EffectType,
	const bool bIsActive,
	const int32 StackCount,
	const float RemainingTime)
{
	RefreshStatusEffectTexts();

	OnStatusEffectUpdated(
		EffectType,
		bIsActive,
		StackCount,
		RemainingTime);
}

void UCh03_GameHUDWidget::BindToGameState()
{
	ACh03_GameStateBase* GameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr;

	if (!GameState)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GameHUDWidget could not find Ch03_GameStateBase."));
		return;
	}

	if (BoundGameState != GameState)
	{
		UnbindFromGameState();
		BoundGameState = GameState;
	}

	BoundGameState->OnScoreChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleScoreChanged);

	BoundGameState->OnWaveChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleWaveChanged);

	BoundGameState->OnRemainingTimeChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleRemainingTimeChanged);

	BoundGameState->OnAnnouncementChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleAnnouncementChanged);

	BoundGameState->OnComboChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleComboChanged);

	BoundGameState->OnComboRewardTriggered.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleComboRewardTriggered);

	BoundGameState->OnComboBroken.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleComboBroken);

	HandleScoreChanged(BoundGameState->GetScore());
	HandleWaveChanged(
		BoundGameState->GetCurrentWave(),
		BoundGameState->GetMaxWave());
	HandleRemainingTimeChanged(
		BoundGameState->GetRemainingTime());
	HandleAnnouncementChanged(
		BoundGameState->GetAnnouncementText());
	HandleComboChanged(
		BoundGameState->GetComboCount(),
		BoundGameState->GetComboTimeRemaining(),
		BoundGameState->GetCurrentComboScoreMultiplier(),
		BoundGameState->GetComboCount() > 0
			&& BoundGameState->GetComboTimeRemaining() > 0.0f);
}

void UCh03_GameHUDWidget::UnbindFromGameState()
{
	if (BoundGameState)
	{
		BoundGameState->OnScoreChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleScoreChanged);

		BoundGameState->OnWaveChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleWaveChanged);

		BoundGameState->OnRemainingTimeChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleRemainingTimeChanged);

		BoundGameState->OnAnnouncementChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleAnnouncementChanged);

		BoundGameState->OnComboChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleComboChanged);

		BoundGameState->OnComboRewardTriggered.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleComboRewardTriggered);

		BoundGameState->OnComboBroken.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleComboBroken);
	}

	BoundGameState = nullptr;
}

void UCh03_GameHUDWidget::BindToCharacter()
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	ACh03_CheonbokCharacter* CheonbokCharacter =
		OwningPlayer
			? Cast<ACh03_CheonbokCharacter>(OwningPlayer->GetPawn())
			: nullptr;

	if (!CheonbokCharacter)
	{
		if (GetWorld()
			&& !GetWorld()->GetTimerManager().IsTimerActive(
				CharacterBindRetryTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(
				CharacterBindRetryTimerHandle,
				this,
				&UCh03_GameHUDWidget::BindToCharacter,
				0.2f,
				true);
		}
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			CharacterBindRetryTimerHandle);
	}

	if (BoundCharacter != CheonbokCharacter)
	{
		UnbindFromCharacter();
		BoundCharacter = CheonbokCharacter;
	}

	BoundCharacter->OnHealthChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleHealthChanged);

	BoundCharacter->OnStaminaChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleStaminaChanged);

	BoundCharacter->OnStatusEffectChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleStatusEffectChanged);

	HandleHealthChanged(
		BoundCharacter->GetHealth(),
		BoundCharacter->GetMaxHealth());

	HandleStaminaChanged(
		BoundCharacter->GetStamina(),
		BoundCharacter->GetMaxStamina());

	RefreshPortraitImage();
	RefreshStatusEffectTexts();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			StatusEffectRefreshTimerHandle,
			this,
			&UCh03_GameHUDWidget::RefreshStatusEffectTexts,
			0.2f,
			true);
	}
}

void UCh03_GameHUDWidget::UnbindFromCharacter()
{
	if (BoundCharacter)
	{
		BoundCharacter->OnHealthChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleHealthChanged);

		BoundCharacter->OnStaminaChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleStaminaChanged);

		BoundCharacter->OnStatusEffectChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleStatusEffectChanged);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			StatusEffectRefreshTimerHandle);
	}

	BoundCharacter = nullptr;
}

void UCh03_GameHUDWidget::RefreshPortraitImage()
{
	UTextureRenderTarget2D* PortraitRenderTarget =
		BoundCharacter ? BoundCharacter->GetPortraitRenderTarget() : nullptr;

	if (CheonbokFaceImage && PortraitRenderTarget)
	{
		FSlateBrush PortraitBrush = CheonbokFaceImage->GetBrush();

		if (UMaterialInstanceDynamic* ResolvedMaterial =
			ResolvePortraitBrushMaterial(PortraitBrush))
		{
			ResolvedMaterial->SetTextureParameterValue(
				PortraitTextureParameterName,
				PortraitRenderTarget);
			PortraitBrush.SetResourceObject(ResolvedMaterial);
		}
		else
		{
			PortraitBrush.SetResourceObject(PortraitRenderTarget);
		}

		PortraitBrush.ImageSize = FVector2D(
			static_cast<float>(PortraitRenderTarget->SizeX),
			static_cast<float>(PortraitRenderTarget->SizeY));
		CheonbokFaceImage->SetBrush(PortraitBrush);
		CheonbokFaceImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (BoundCharacter)
	{
		BoundCharacter->RefreshPortraitCapture();
	}

	OnPortraitRenderTargetUpdated(PortraitRenderTarget);
}

UMaterialInstanceDynamic* UCh03_GameHUDWidget::ResolvePortraitBrushMaterial(
	const FSlateBrush& SourceBrush)
{
	if (PortraitBrushMaterial)
	{
		if (!PortraitBrushMaterialInstance)
		{
			PortraitBrushMaterialInstance =
				UMaterialInstanceDynamic::Create(PortraitBrushMaterial, this);
		}

		return PortraitBrushMaterialInstance;
	}

	UObject* BrushResource = SourceBrush.GetResourceObject();
	if (UMaterialInstanceDynamic* ExistingDynamicMaterial =
		Cast<UMaterialInstanceDynamic>(BrushResource))
	{
		PortraitBrushMaterialInstance = ExistingDynamicMaterial;
		return PortraitBrushMaterialInstance;
	}

	if (UMaterialInterface* PortraitMaterial =
		Cast<UMaterialInterface>(BrushResource))
	{
		PortraitBrushMaterialInstance =
			UMaterialInstanceDynamic::Create(PortraitMaterial, this);
		return PortraitBrushMaterialInstance;
	}

	return nullptr;
}

void UCh03_GameHUDWidget::CreateWaveBannerTextFallback()
{
	if (!WidgetTree || WaveBannerText)
	{
		return;
	}

	UPanelWidget* RootPanel =
		Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		return;
	}

	WaveBannerText =
		WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			TEXT("WaveBannerText_NativeFallback"));

	WaveBannerText->SetVisibility(ESlateVisibility::Collapsed);
	WaveBannerText->SetJustification(ETextJustify::Center);
	WaveBannerText->SetAutoWrapText(true);
	WaveBannerText->SetColorAndOpacity(
		FSlateColor(FLinearColor(1.0f, 0.88f, 0.35f, 1.0f)));
	WaveBannerText->SetShadowOffset(FVector2D(2.0f, 2.0f));
	WaveBannerText->SetShadowColorAndOpacity(
		FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));

	FSlateFontInfo BannerFont = WaveBannerText->GetFont();
	BannerFont.Size = 48;
	WaveBannerText->SetFont(BannerFont);

	if (UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(RootPanel))
	{
		UCanvasPanelSlot* CanvasSlot =
			RootCanvas->AddChildToCanvas(WaveBannerText);
		CanvasSlot->SetAnchors(FAnchors(0.5f, 0.35f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetSize(FVector2D(720.0f, 120.0f));
		CanvasSlot->SetZOrder(50);
	}
	else
	{
		RootPanel->AddChild(WaveBannerText);
	}
}

void UCh03_GameHUDWidget::CreateStaminaFallbacks()
{
	if (!WidgetTree || (StaminaText && StaminaBar))
	{
		return;
	}

	UPanelWidget* RootPanel =
		Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		return;
	}

	UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(RootPanel);

	if (!StaminaBar)
	{
		StaminaBar = WidgetTree->ConstructWidget<UProgressBar>(
			UProgressBar::StaticClass(),
			TEXT("StaminaBar_NativeFallback"));
		StaminaBar->SetPercent(1.0f);
		StaminaBar->SetFillColorAndOpacity(
			FLinearColor(0.35f, 0.78f, 1.0f, 1.0f));

		if (RootCanvas)
		{
			UCanvasPanelSlot* BarSlot =
				RootCanvas->AddChildToCanvas(StaminaBar);
			BarSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			BarSlot->SetAlignment(FVector2D(0.0f, 0.0f));
			BarSlot->SetPosition(FVector2D(32.0f, 74.0f));
			BarSlot->SetSize(FVector2D(220.0f, 16.0f));
			BarSlot->SetZOrder(10);
		}
		else
		{
			RootPanel->AddChild(StaminaBar);
		}
	}

	if (!StaminaText)
	{
		StaminaText = WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			TEXT("StaminaText_NativeFallback"));
		StaminaText->SetColorAndOpacity(
			FSlateColor(FLinearColor(0.82f, 0.95f, 1.0f, 1.0f)));
		StaminaText->SetShadowOffset(FVector2D(1.0f, 1.0f));
		StaminaText->SetShadowColorAndOpacity(
			FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));

		if (RootCanvas)
		{
			UCanvasPanelSlot* TextSlot =
				RootCanvas->AddChildToCanvas(StaminaText);
			TextSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			TextSlot->SetAlignment(FVector2D(0.0f, 0.0f));
			TextSlot->SetPosition(FVector2D(32.0f, 48.0f));
			TextSlot->SetSize(FVector2D(260.0f, 24.0f));
			TextSlot->SetZOrder(10);
		}
		else
		{
			RootPanel->AddChild(StaminaText);
		}
	}
}

void UCh03_GameHUDWidget::CreateComboFallbacks()
{
	if (!WidgetTree || (ComboText && ComboRewardText))
	{
		return;
	}

	UPanelWidget* RootPanel =
		Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		return;
	}

	if (UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(RootPanel))
	{
		if (!ComboText)
		{
			ComboText = WidgetTree->ConstructWidget<UTextBlock>(
				UTextBlock::StaticClass(),
				TEXT("ComboText_NativeFallback"));

			ComboText->SetVisibility(ESlateVisibility::Collapsed);
			ComboText->SetJustification(ETextJustify::Left);
			ComboText->SetColorAndOpacity(
				FSlateColor(FLinearColor(1.0f, 0.86f, 0.16f, 1.0f)));
			ComboText->SetShadowOffset(FVector2D(1.5f, 1.5f));
			ComboText->SetShadowColorAndOpacity(
				FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));

			FSlateFontInfo ComboFont = ComboText->GetFont();
			ComboFont.Size = 26;
			ComboText->SetFont(ComboFont);

			UCanvasPanelSlot* CanvasSlot =
				RootCanvas->AddChildToCanvas(ComboText);
			CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			CanvasSlot->SetAlignment(FVector2D(0.0f, 0.0f));
			CanvasSlot->SetPosition(FVector2D(32.0f, 98.0f));
			CanvasSlot->SetSize(FVector2D(360.0f, 40.0f));
			CanvasSlot->SetZOrder(20);
		}

		if (!ComboRewardText)
		{
			ComboRewardText = WidgetTree->ConstructWidget<UTextBlock>(
				UTextBlock::StaticClass(),
				TEXT("ComboRewardText_NativeFallback"));

			ComboRewardText->SetVisibility(ESlateVisibility::Collapsed);
			ComboRewardText->SetJustification(ETextJustify::Left);
			ComboRewardText->SetColorAndOpacity(
				FSlateColor(FLinearColor(1.0f, 0.55f, 0.12f, 1.0f)));
			ComboRewardText->SetShadowOffset(FVector2D(1.5f, 1.5f));
			ComboRewardText->SetShadowColorAndOpacity(
				FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));

			FSlateFontInfo RewardFont = ComboRewardText->GetFont();
			RewardFont.Size = 24;
			ComboRewardText->SetFont(RewardFont);

			UCanvasPanelSlot* RewardSlot =
				RootCanvas->AddChildToCanvas(ComboRewardText);
			RewardSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			RewardSlot->SetAlignment(FVector2D(0.0f, 0.0f));
			RewardSlot->SetPosition(FVector2D(32.0f, 132.0f));
			RewardSlot->SetSize(FVector2D(540.0f, 40.0f));
			RewardSlot->SetZOrder(21);
		}
	}
	else
	{
		if (!ComboText)
		{
			ComboText = WidgetTree->ConstructWidget<UTextBlock>(
				UTextBlock::StaticClass(),
				TEXT("ComboText_NativeFallback"));
			ComboText->SetVisibility(ESlateVisibility::Collapsed);
			RootPanel->AddChild(ComboText);
		}

		if (!ComboRewardText)
		{
			ComboRewardText = WidgetTree->ConstructWidget<UTextBlock>(
				UTextBlock::StaticClass(),
				TEXT("ComboRewardText_NativeFallback"));
			ComboRewardText->SetVisibility(ESlateVisibility::Collapsed);
			RootPanel->AddChild(ComboRewardText);
		}
	}
}

void UCh03_GameHUDWidget::UpdateComboRewardFeedback(
	const float DeltaTime)
{
	if (!ComboRewardText || ComboRewardRemainingTime <= 0.0f)
	{
		return;
	}

	ComboRewardRemainingTime = FMath::Max(
		0.0f,
		ComboRewardRemainingTime - FMath::Max(0.0f, DeltaTime));

	const float FadeStartTime = 0.35f;
	const float Opacity = ComboRewardRemainingTime < FadeStartTime
		? FMath::Clamp(ComboRewardRemainingTime / FadeStartTime, 0.0f, 1.0f)
		: 1.0f;

	ComboRewardText->SetRenderOpacity(Opacity);

	if (ComboRewardRemainingTime <= 0.0f)
	{
		ComboRewardText->SetText(FText::GetEmpty());
		ComboRewardText->SetVisibility(ESlateVisibility::Collapsed);
		ComboRewardText->SetRenderOpacity(1.0f);
	}
}

void UCh03_GameHUDWidget::PlayWaveBannerIntroAnimation()
{
	if (!WaveBannerText)
	{
		return;
	}

	WaveBannerAnimationTime = 0.0f;
	bIsWaveBannerAnimationActive = true;
	bIsWaveBannerOutroAnimation = false;

	WaveBannerText->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	ApplyWaveBannerAnimation(0.0f, false);
}

void UCh03_GameHUDWidget::PlayWaveBannerOutroAnimation()
{
	if (!WaveBannerText)
	{
		return;
	}

	if (WaveBannerText->GetVisibility() == ESlateVisibility::Collapsed
		|| WaveBannerText->GetText().IsEmpty())
	{
		FinishWaveBannerOutroAnimation();
		return;
	}

	WaveBannerAnimationTime = 0.0f;
	bIsWaveBannerAnimationActive = true;
	bIsWaveBannerOutroAnimation = true;

	WaveBannerText->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	ApplyWaveBannerAnimation(0.0f, true);
}

void UCh03_GameHUDWidget::UpdateWaveBannerAnimation(
	const float DeltaTime)
{
	if (!bIsWaveBannerAnimationActive || !WaveBannerText)
	{
		return;
	}

	const float Duration = bIsWaveBannerOutroAnimation
		? WaveBannerOutroDuration
		: WaveBannerIntroDuration;

	WaveBannerAnimationTime += FMath::Max(0.0f, DeltaTime);
	const float Alpha = Duration > 0.0f
		? FMath::Clamp(WaveBannerAnimationTime / Duration, 0.0f, 1.0f)
		: 1.0f;

	ApplyWaveBannerAnimation(Alpha, bIsWaveBannerOutroAnimation);

	if (Alpha < 1.0f)
	{
		return;
	}

	if (bIsWaveBannerOutroAnimation)
	{
		FinishWaveBannerOutroAnimation();
	}
	else
	{
		bIsWaveBannerAnimationActive = false;
		WaveBannerAnimationTime = 0.0f;
		ApplyWaveBannerAnimation(1.0f, false);
	}
}

void UCh03_GameHUDWidget::ApplyWaveBannerAnimation(
	const float Alpha,
	const bool bIsOutro)
{
	if (!WaveBannerText)
	{
		return;
	}

	const float SmoothedAlpha =
		FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);

	const float Opacity = bIsOutro
		? 1.0f - SmoothedAlpha
		: SmoothedAlpha;
	const float Scale = bIsOutro
		? FMath::Lerp(1.0f, 0.96f, SmoothedAlpha)
		: FMath::Lerp(1.16f, 1.0f, SmoothedAlpha);

	WaveBannerText->SetRenderOpacity(Opacity);
	WaveBannerText->SetRenderScale(FVector2D(Scale, Scale));
}

void UCh03_GameHUDWidget::FinishWaveBannerOutroAnimation()
{
	if (WaveBannerText)
	{
		WaveBannerText->SetText(FText::GetEmpty());
		WaveBannerText->SetRenderOpacity(0.0f);
		WaveBannerText->SetRenderScale(FVector2D(1.0f, 1.0f));
		WaveBannerText->SetVisibility(ESlateVisibility::Collapsed);
	}

	bIsWaveBannerAnimationActive = false;
	bIsWaveBannerOutroAnimation = false;
	WaveBannerAnimationTime = 0.0f;
}

void UCh03_GameHUDWidget::CreateStatusEffectTextFallbacks()
{
	if (StatusEffectPanel)
	{
		UpdateStatusEffectText(
			SlowStatusText,
			NSLOCTEXT("CheonbokHUD", "SlowStatus", "Slow"),
			false,
			0,
			0.0f);
		UpdateStatusEffectText(
			ReverseControlStatusText,
			NSLOCTEXT("CheonbokHUD", "ReverseStatus", "Reverse"),
			false,
			0,
			0.0f);
		UpdateStatusEffectText(
			MovementLockStatusText,
			NSLOCTEXT("CheonbokHUD", "MovementLockStatus", "Rooted"),
			false,
			0,
			0.0f);
		UpdateStatusEffectText(
			DamageShieldStatusText,
			NSLOCTEXT("CheonbokHUD", "DamageShieldStatus", "Shield"),
			false,
			0,
			-1.0f);
		return;
	}

	if (!WidgetTree
		|| (SlowStatusText
			&& ReverseControlStatusText
			&& MovementLockStatusText
			&& DamageShieldStatusText))
	{
		return;
	}

	UPanelWidget* RootPanel =
		Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!RootPanel)
	{
		return;
	}

	auto CreateStatusText =
		[this, RootPanel](
			const FName WidgetName,
			const FVector2D Position) -> UTextBlock*
		{
			UTextBlock* StatusText =
				WidgetTree->ConstructWidget<UTextBlock>(
					UTextBlock::StaticClass(),
					WidgetName);

			StatusText->SetVisibility(ESlateVisibility::Collapsed);
			StatusText->SetColorAndOpacity(
				FSlateColor(FLinearColor(1.0f, 0.65f, 0.25f, 1.0f)));
			StatusText->SetShadowOffset(FVector2D(1.5f, 1.5f));
			StatusText->SetShadowColorAndOpacity(
				FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));

			if (UCanvasPanel* RootCanvas =
				Cast<UCanvasPanel>(RootPanel))
			{
				UCanvasPanelSlot* CanvasSlot =
					RootCanvas->AddChildToCanvas(StatusText);
				CanvasSlot->SetAnchors(FAnchors(0.0f, 1.0f));
				CanvasSlot->SetAlignment(FVector2D(0.0f, 1.0f));
				CanvasSlot->SetPosition(Position);
				CanvasSlot->SetSize(FVector2D(320.0f, 32.0f));
			}
			else
			{
				RootPanel->AddChild(StatusText);
			}

			return StatusText;
		};

	if (!SlowStatusText)
	{
		SlowStatusText = CreateStatusText(
			TEXT("SlowStatusText_NativeFallback"),
			FVector2D(32.0f, -140.0f));
	}

	if (!ReverseControlStatusText)
	{
		ReverseControlStatusText = CreateStatusText(
			TEXT("ReverseControlStatusText_NativeFallback"),
			FVector2D(32.0f, -104.0f));
	}

	if (!MovementLockStatusText)
	{
		MovementLockStatusText = CreateStatusText(
			TEXT("MovementLockStatusText_NativeFallback"),
			FVector2D(32.0f, -68.0f));
	}

	if (!DamageShieldStatusText)
	{
		DamageShieldStatusText = CreateStatusText(
			TEXT("DamageShieldStatusText_NativeFallback"),
			FVector2D(32.0f, -32.0f));
	}
}

void UCh03_GameHUDWidget::RefreshStatusEffectTexts()
{
	if (!BoundCharacter)
	{
		UpdateStatusEffect(
			SlowStatusText,
			ECheonbokStatusEffect::Slow,
			NSLOCTEXT("CheonbokHUD", "SlowStatus", "Slow"),
			false,
			0,
			0.0f);
		UpdateStatusEffect(
			ReverseControlStatusText,
			ECheonbokStatusEffect::ReverseControl,
			NSLOCTEXT("CheonbokHUD", "ReverseStatus", "Reverse"),
			false,
			0,
			0.0f);
		UpdateStatusEffect(
			MovementLockStatusText,
			ECheonbokStatusEffect::MovementLock,
			NSLOCTEXT("CheonbokHUD", "MovementLockStatus", "Rooted"),
			false,
			0,
			0.0f);
		UpdateStatusEffect(
			DamageShieldStatusText,
			ECheonbokStatusEffect::DamageShield,
			NSLOCTEXT("CheonbokHUD", "DamageShieldStatus", "Shield"),
			false,
			0,
			-1.0f);
		return;
	}

	UpdateStatusEffect(
		SlowStatusText,
		ECheonbokStatusEffect::Slow,
		NSLOCTEXT("CheonbokHUD", "SlowStatus", "Slow"),
		BoundCharacter->IsSlowActive(),
		BoundCharacter->GetSlowStackCount(),
		BoundCharacter->GetSlowRemainingTime());

	UpdateStatusEffect(
		ReverseControlStatusText,
		ECheonbokStatusEffect::ReverseControl,
		NSLOCTEXT("CheonbokHUD", "ReverseStatus", "Reverse"),
		BoundCharacter->IsReverseControlActive(),
		BoundCharacter->GetReverseControlStackCount(),
		BoundCharacter->GetReverseControlRemainingTime());

	UpdateStatusEffect(
		MovementLockStatusText,
		ECheonbokStatusEffect::MovementLock,
		NSLOCTEXT("CheonbokHUD", "MovementLockStatus", "Rooted"),
		BoundCharacter->IsMovementLockActive(),
		BoundCharacter->GetMovementLockStackCount(),
		BoundCharacter->GetMovementLockRemainingTime());

	UpdateStatusEffect(
		DamageShieldStatusText,
		ECheonbokStatusEffect::DamageShield,
		NSLOCTEXT("CheonbokHUD", "DamageShieldStatus", "Shield"),
		BoundCharacter->IsDamageShieldActive(),
		BoundCharacter->GetDamageShieldStackCount(),
		-1.0f);
}

void UCh03_GameHUDWidget::UpdateStatusEffect(
	UTextBlock* TargetText,
	const ECheonbokStatusEffect EffectType,
	const FText& Label,
	const bool bIsActive,
	const int32 StackCount,
	const float RemainingTime)
{
	if (StatusEffectPanel)
	{
		StatusEffectPanel->UpdateStatusEffect(
			EffectType,
			Label,
			bIsActive,
			StackCount,
			RemainingTime);
	}

	UpdateStatusEffectText(
		TargetText,
		Label,
		!StatusEffectPanel && bIsActive,
		StackCount,
		RemainingTime);
}

void UCh03_GameHUDWidget::UpdateStatusEffectText(
	UTextBlock* TargetText,
	const FText& Label,
	const bool bIsActive,
	const int32 StackCount,
	const float RemainingTime)
{
	if (!TargetText)
	{
		return;
	}

	TargetText->SetVisibility(
		bIsActive
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);

	if (!bIsActive)
	{
		TargetText->SetText(FText::GetEmpty());
		return;
	}

	if (RemainingTime >= 0.0f)
	{
		TargetText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"StatusEffectFormat",
					"{0} x{1}  {2}s"),
				Label,
				FText::AsNumber(FMath::Max(1, StackCount)),
				FText::AsNumber(FMath::CeilToInt(RemainingTime))));
		return;
	}

	TargetText->SetText(
		FText::Format(
			NSLOCTEXT(
				"CheonbokHUD",
				"StatusEffectStackFormat",
				"{0} x{1}"),
			Label,
			FText::AsNumber(FMath::Max(1, StackCount))));
}
