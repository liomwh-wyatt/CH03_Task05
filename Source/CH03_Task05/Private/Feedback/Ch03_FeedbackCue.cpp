// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Feedback/Ch03_FeedbackCue.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

void UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtActor(
	const UObject* WorldContextObject,
	const FCh03FeedbackCue& FeedbackCue,
	AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	const FVector Location =
		TargetActor->GetActorLocation() + FeedbackCue.LocationOffset;
	const FRotator Rotation =
		TargetActor->GetActorRotation() + FeedbackCue.RotationOffset;

	if (FeedbackCue.Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			WorldContextObject,
			FeedbackCue.Sound,
			Location,
			Rotation,
			FeedbackCue.SoundVolumeMultiplier,
			FeedbackCue.SoundPitchMultiplier);
	}

	if (!FeedbackCue.Effect)
	{
		return;
	}

	if (FeedbackCue.bAttachEffectToTarget
		&& TargetActor->GetRootComponent())
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			FeedbackCue.Effect,
			TargetActor->GetRootComponent(),
			NAME_None,
			FeedbackCue.LocationOffset,
			FeedbackCue.RotationOffset,
			EAttachLocation::KeepRelativeOffset,
			true);
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		WorldContextObject,
		FeedbackCue.Effect,
		Location,
		Rotation);
}

void UCh03_FeedbackFunctionLibrary::PlayFeedbackCueAtLocation(
	const UObject* WorldContextObject,
	const FCh03FeedbackCue& FeedbackCue,
	const FVector Location,
	const FRotator Rotation)
{
	const FVector AdjustedLocation = Location + FeedbackCue.LocationOffset;
	const FRotator AdjustedRotation = Rotation + FeedbackCue.RotationOffset;

	if (FeedbackCue.Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			WorldContextObject,
			FeedbackCue.Sound,
			AdjustedLocation,
			AdjustedRotation,
			FeedbackCue.SoundVolumeMultiplier,
			FeedbackCue.SoundPitchMultiplier);
	}

	if (FeedbackCue.Effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			WorldContextObject,
			FeedbackCue.Effect,
			AdjustedLocation,
			AdjustedRotation);
	}
}
