// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Core/Ch03_GameStateBase.h"

#include "Character/Ch03_CheonbokCharacter.h"

ACh03_GameStateBase::ACh03_GameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentScore = 0;
	CurrentWave = 0;
	MaxWave = 3;
	RemainingTime = 0;
	WaveDuration = 0;
	AnnouncementText = FText::GetEmpty();
}

void ACh03_GameStateBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentComboCount <= 0 || ComboTimeRemaining <= 0.0f)
	{
		return;
	}

	ComboTimeRemaining = FMath::Max(
		0.0f,
		ComboTimeRemaining - FMath::Max(0.0f, DeltaSeconds));

	if (ComboTimeRemaining <= 0.0f)
	{
		ResetComboState(
			true,
			ECh03ComboBreakReason::Timeout,
			true);
		return;
	}

	BroadcastComboChanged();
}

void ACh03_GameStateBase::AddScore(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok score: %d (+%d)"),
		CurrentScore,
		Amount);
}

int32 ACh03_GameStateBase::AddComboScore(
	const int32 BaseAmount,
	AActor* ScoringActor)
{
	if (BaseAmount <= 0)
	{
		return 0;
	}

	if (ComboTimeRemaining <= 0.0f)
	{
		ResetComboState(
			false,
			ECh03ComboBreakReason::Timeout,
			false);
	}

	++CurrentComboCount;
	ComboTimeRemaining = ComboWindowSeconds;
	BestComboCount = FMath::Max(BestComboCount, CurrentComboCount);

	CurrentComboScoreMultiplier =
		GetScoreMultiplierForCombo(CurrentComboCount);

	float TotalMultiplier = CurrentComboScoreMultiplier;
	if (bNextScoreItemDouble)
	{
		TotalMultiplier *= 2.0f;
		bNextScoreItemDouble = false;
	}

	const int32 FinalAmount = FMath::Max(
		1,
		FMath::RoundToInt(static_cast<float>(BaseAmount) * TotalMultiplier));

	AddScore(FinalAmount);
	ProcessComboRewards(CurrentComboCount, ScoringActor);
	BroadcastComboChanged();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok combo score: Combo=%d, Base=%d, Final=%d, Multiplier=%.2f"),
		CurrentComboCount,
		BaseAmount,
		FinalAmount,
		TotalMultiplier);

	return FinalAmount;
}

void ACh03_GameStateBase::ResetScore()
{
	SetScore(0);
	ResetComboStats();
}

void ACh03_GameStateBase::SetScore(
	const int32 NewScore)
{
	const int32 SanitizedScore = FMath::Max(0, NewScore);

	if (CurrentScore == SanitizedScore)
	{
		return;
	}

	CurrentScore = SanitizedScore;
	OnScoreChanged.Broadcast(CurrentScore);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok score set: %d"),
		CurrentScore);
}

void ACh03_GameStateBase::SetWave(
	const int32 NewCurrentWave,
	const int32 NewMaxWave)
{
	const int32 SanitizedMaxWave = FMath::Max(1, NewMaxWave);
	const int32 SanitizedCurrentWave = FMath::Clamp(
		NewCurrentWave,
		0,
		SanitizedMaxWave);

	if (CurrentWave == SanitizedCurrentWave
		&& MaxWave == SanitizedMaxWave)
	{
		return;
	}

	CurrentWave = SanitizedCurrentWave;
	MaxWave = SanitizedMaxWave;
	OnWaveChanged.Broadcast(CurrentWave, MaxWave);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok wave: %d / %d"),
		CurrentWave,
		MaxWave);
}

void ACh03_GameStateBase::SetRemainingTime(
	const int32 NewRemainingTime)
{
	const int32 SanitizedRemainingTime = FMath::Max(
		0,
		NewRemainingTime);

	if (RemainingTime == SanitizedRemainingTime)
	{
		return;
	}

	RemainingTime = SanitizedRemainingTime;
	OnRemainingTimeChanged.Broadcast(RemainingTime);

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("Cheonbok remaining time: %d"),
		RemainingTime);
}

void ACh03_GameStateBase::SetWaveDuration(
	const int32 NewWaveDuration)
{
	const int32 SanitizedWaveDuration = FMath::Max(
		0,
		NewWaveDuration);

	if (WaveDuration == SanitizedWaveDuration)
	{
		return;
	}

	WaveDuration = SanitizedWaveDuration;

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("Cheonbok wave duration: %d"),
		WaveDuration);
}

void ACh03_GameStateBase::SetAnnouncementText(
	const FText& NewAnnouncement)
{
	if (AnnouncementText.EqualTo(NewAnnouncement))
	{
		return;
	}

	AnnouncementText = NewAnnouncement;
	OnAnnouncementChanged.Broadcast(AnnouncementText);
}

void ACh03_GameStateBase::ClearAnnouncementText()
{
	SetAnnouncementText(FText::GetEmpty());
}

void ACh03_GameStateBase::BreakCombo()
{
	BreakComboWithReason(ECh03ComboBreakReason::Manual);
}

void ACh03_GameStateBase::BreakComboWithReason(
	const ECh03ComboBreakReason BreakReason)
{
	ResetComboState(
		true,
		BreakReason,
		true);
}

void ACh03_GameStateBase::ResetComboStats()
{
	ResetComboState(
		true,
		ECh03ComboBreakReason::GameFlow,
		false);
	BestComboCount = 0;
}

float ACh03_GameStateBase::GetScoreMultiplierForCombo(
	const int32 ComboCount) const
{
	if (ComboCount >= 12)
	{
		return 2.0f;
	}

	if (ComboCount >= 8)
	{
		return 1.5f;
	}

	if (ComboCount >= 5)
	{
		return 1.25f;
	}

	if (ComboCount >= 3)
	{
		return 1.1f;
	}

	return 1.0f;
}

void ACh03_GameStateBase::ProcessComboRewards(
	const int32 ComboCount,
	AActor* ScoringActor)
{
	if (RewardedComboThresholds.Contains(ComboCount))
	{
		return;
	}

	auto BroadcastReward =
		[this, ComboCount](const FText& RewardText)
		{
			RewardedComboThresholds.Add(ComboCount);
			OnComboRewardTriggered.Broadcast(ComboCount, RewardText);

			UE_LOG(
				LogTemp,
				Log,
				TEXT("Cheonbok combo reward: Combo=%d, Reward=%s"),
				ComboCount,
				*RewardText.ToString());
		};

	if (ComboCount == 3)
	{
		BroadcastReward(
			NSLOCTEXT(
				"CheonbokCombo",
				"Combo3Reward",
				"우다다 x3! 점수 +10%"));
		return;
	}

	if (ComboCount == 5)
	{
		if (ACh03_CheonbokCharacter* CheonbokCharacter =
			Cast<ACh03_CheonbokCharacter>(ScoringActor))
		{
			CheonbokCharacter->AddStamina(5.0f);
		}

		BroadcastReward(
			NSLOCTEXT(
				"CheonbokCombo",
				"Combo5Reward",
				"우다다 x5! 점수 +25%, 스태미나 +5"));
		return;
	}

	if (ComboCount == 8)
	{
		if (ACh03_CheonbokCharacter* CheonbokCharacter =
			Cast<ACh03_CheonbokCharacter>(ScoringActor))
		{
			CheonbokCharacter->AddStamina(10.0f);
		}

		BroadcastReward(
			NSLOCTEXT(
				"CheonbokCombo",
				"Combo8Reward",
				"우다다 x8! 점수 +50%, 스태미나 +10"));
		return;
	}

	if (ComboCount == 10)
	{
		bNextScoreItemDouble = true;

		BroadcastReward(
			NSLOCTEXT(
				"CheonbokCombo",
				"Combo10Reward",
				"우다다 x10! 다음 간식 점수 2배"));
		return;
	}

	if (ComboCount == 12)
	{
		const bool bShouldRequestGoldenItem =
			FMath::FRand() <= Combo12GoldenItemChance;
		if (bShouldRequestGoldenItem)
		{
			OnGoldenItemRequested.Broadcast(ComboCount);
		}

		BroadcastReward(
			bShouldRequestGoldenItem
				? NSLOCTEXT(
					"CheonbokCombo",
					"Combo12GoldenReward",
					"우다다 x12! 황금 날개 사료 등장!")
				: NSLOCTEXT(
					"CheonbokCombo",
					"Combo12Reward",
					"우다다 x12! 황금 날개 사료 등장 기회"));
		return;
	}

	if (ComboCount == 15)
	{
		OnGoldenItemRequested.Broadcast(ComboCount);

		BroadcastReward(
			NSLOCTEXT(
				"CheonbokCombo",
				"Combo15Reward",
				"우다다 x15! 황금 날개 사료 확정 등장!"));
	}
}

void ACh03_GameStateBase::ResetComboState(
	const bool bShouldBroadcast,
	const ECh03ComboBreakReason BreakReason,
	const bool bShouldBroadcastBreak)
{
	const int32 PreviousComboCount = CurrentComboCount;
	const bool bHadCombo = CurrentComboCount > 0
		|| ComboTimeRemaining > 0.0f
		|| !FMath::IsNearlyEqual(CurrentComboScoreMultiplier, 1.0f)
		|| bNextScoreItemDouble
		|| !RewardedComboThresholds.IsEmpty();

	CurrentComboCount = 0;
	ComboTimeRemaining = 0.0f;
	CurrentComboScoreMultiplier = 1.0f;
	bNextScoreItemDouble = false;
	RewardedComboThresholds.Reset();

	if (bShouldBroadcastBreak && PreviousComboCount >= 2)
	{
		OnComboBroken.Broadcast(PreviousComboCount, BreakReason);
	}

	if (bShouldBroadcast && bHadCombo)
	{
		BroadcastComboChanged();
	}
}

void ACh03_GameStateBase::BroadcastComboChanged()
{
	OnComboChanged.Broadcast(
		CurrentComboCount,
		ComboTimeRemaining,
		CurrentComboScoreMultiplier,
		CurrentComboCount > 0 && ComboTimeRemaining > 0.0f);
}
