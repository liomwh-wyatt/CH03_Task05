// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Ch03_FeedbackCue.generated.h"

class AActor;
class UNiagaraSystem;
class USoundBase;

USTRUCT(BlueprintType)
struct CH03_TASK05_API FCh03FeedbackCue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback")
	TObjectPtr<UNiagaraSystem> Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback")
	bool bAttachEffectToTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ClampMin = "0.0"))
	float SoundVolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ClampMin = "0.0"))
	float SoundPitchMultiplier = 1.0f;
};

UCLASS()
class CH03_TASK05_API UCh03_FeedbackFunctionLibrary
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Feedback",
		meta = (WorldContext = "WorldContextObject"))
	static void PlayFeedbackCueAtActor(
		const UObject* WorldContextObject,
		const FCh03FeedbackCue& FeedbackCue,
		AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Feedback",
		meta = (WorldContext = "WorldContextObject"))
	static void PlayFeedbackCueAtLocation(
		const UObject* WorldContextObject,
		const FCh03FeedbackCue& FeedbackCue,
		FVector Location,
		FRotator Rotation = FRotator::ZeroRotator);
};
