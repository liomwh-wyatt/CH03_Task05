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
	if (BoundGameState)
	{
		BoundGameState->OnScoreChanged.RemoveDynamic(
			this,
			&UCh03_GameHUDWidget::HandleScoreChanged);
	}

	BoundGameState = nullptr;
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

void UCh03_GameHUDWidget::BindToGameState()
{
	BoundGameState =
		GetWorld() ? GetWorld()->GetGameState<ACh03_GameStateBase>() : nullptr;

	if (!BoundGameState)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GameHUDWidget could not find Ch03_GameStateBase."));
		return;
	}

	BoundGameState->OnScoreChanged.AddUniqueDynamic(
		this,
		&UCh03_GameHUDWidget::HandleScoreChanged);

	HandleScoreChanged(BoundGameState->GetScore());
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
