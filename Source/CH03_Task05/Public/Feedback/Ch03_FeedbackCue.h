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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ToolTip = "피드백이 재생될 때 함께 출력할 사운드입니다. 비워두면 사운드를 재생하지 않습니다."))
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ToolTip = "피드백이 재생될 때 사용할 Niagara 이펙트입니다. 비워두면 이펙트를 재생하지 않습니다."))
	TObjectPtr<UNiagaraSystem> Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ToolTip = "켜면 이펙트를 대상 액터에 붙여서 재생합니다. 끄면 재생 순간의 월드 위치에 생성합니다."))
	bool bAttachEffectToTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ToolTip = "대상 위치 기준 이펙트와 사운드를 얼마나 이동해서 재생할지 정합니다."))
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ToolTip = "대상 회전 기준 이펙트와 사운드를 얼마나 회전해서 재생할지 정합니다."))
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ClampMin = "0.0", ToolTip = "사운드 볼륨 배율입니다. 1.0은 원본 볼륨입니다."))
	float SoundVolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Feedback",
		meta = (ClampMin = "0.0", ToolTip = "사운드 피치 배율입니다. 1.0은 원본 피치입니다."))
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
