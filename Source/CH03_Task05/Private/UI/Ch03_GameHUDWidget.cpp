// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/Ch03_GameStateBase.h"
#include "GameFramework/PlayerController.h"

void UCh03_GameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateStatusEffectTextFallbacks();
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

void UCh03_GameHUDWidget::HandleScoreChanged(int32 NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(
			FText::Format(
				NSLOCTEXT(
					"CheonbokHUD",
					"ScoreFormat",
					"Snack Score: {0}"),
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

void UCh03_GameHUDWidget::HandleWaveChanged(
	const int32 CurrentWave,
	const int32 MaxWave)
{
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

	OnRemainingTimeUpdated(SafeRemainingTime);
}

void UCh03_GameHUDWidget::HandleAnnouncementChanged(
	const FText NewAnnouncement)
{
	const bool bIsVisible = !NewAnnouncement.IsEmpty();

	if (WaveBannerText)
	{
		WaveBannerText->SetText(NewAnnouncement);
		WaveBannerText->SetVisibility(
			bIsVisible
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed);
	}

	OnAnnouncementUpdated(NewAnnouncement, bIsVisible);
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

	HandleScoreChanged(BoundGameState->GetScore());
	HandleWaveChanged(
		BoundGameState->GetCurrentWave(),
		BoundGameState->GetMaxWave());
	HandleRemainingTimeChanged(
		BoundGameState->GetRemainingTime());
	HandleAnnouncementChanged(
		BoundGameState->GetAnnouncementText());
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

	BoundCharacter->OnStatusEffectChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleStatusEffectChanged);

	HandleHealthChanged(
		BoundCharacter->GetHealth(),
		BoundCharacter->GetMaxHealth());

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

void UCh03_GameHUDWidget::CreateStatusEffectTextFallbacks()
{
	if (!WidgetTree || (SlowStatusText && ReverseControlStatusText))
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
			FVector2D(32.0f, -104.0f));
	}

	if (!ReverseControlStatusText)
	{
		ReverseControlStatusText = CreateStatusText(
			TEXT("ReverseControlStatusText_NativeFallback"),
			FVector2D(32.0f, -68.0f));
	}
}

void UCh03_GameHUDWidget::RefreshStatusEffectTexts()
{
	if (!BoundCharacter)
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
		return;
	}

	UpdateStatusEffectText(
		SlowStatusText,
		NSLOCTEXT("CheonbokHUD", "SlowStatus", "Slow"),
		BoundCharacter->IsSlowActive(),
		BoundCharacter->GetSlowStackCount(),
		BoundCharacter->GetSlowRemainingTime());

	UpdateStatusEffectText(
		ReverseControlStatusText,
		NSLOCTEXT("CheonbokHUD", "ReverseStatus", "Reverse"),
		BoundCharacter->IsReverseControlActive(),
		BoundCharacter->GetReverseControlStackCount(),
		BoundCharacter->GetReverseControlRemainingTime());
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

	TargetText->SetText(
		FText::Format(
			NSLOCTEXT(
				"CheonbokHUD",
				"StatusEffectFormat",
				"{0} x{1}  {2}s"),
			Label,
			FText::AsNumber(FMath::Max(1, StackCount)),
			FText::AsNumber(FMath::CeilToInt(RemainingTime))));
}
