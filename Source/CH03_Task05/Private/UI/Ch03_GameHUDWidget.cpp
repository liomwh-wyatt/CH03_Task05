// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "UI/Ch03_GameHUDWidget.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/Ch03_GameStateBase.h"
#include "GameFramework/PlayerController.h"

void UCh03_GameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

	HandleHealthChanged(
		BoundCharacter->GetHealth(),
		BoundCharacter->GetMaxHealth());
}

void UCh03_GameHUDWidget::UnbindFromCharacter()
{
	if (BoundCharacter)
	{
		BoundCharacter->OnHealthChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleHealthChanged);
	}

	BoundCharacter = nullptr;
}
