// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Ch03_CheonbokAnimation.generated.h"

class ACh03_CheonbokCharacter;
class UCharacterMovementComponent;

UCLASS()
class CH03_TASK05_API UCh03_CheonbokAnimation : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void CacheCharacterReferences();
	void ResetAnimationState();

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|참조")
	TObjectPtr<ACh03_CheonbokCharacter> CheonbokCharacter;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|참조")
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|이동")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|이동")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|이동")
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|이동")
	float VerticalSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|이동")
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|이동")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "천복|상태")
	bool bIsDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|설정",
		meta = (DisplayName = "최소 이동 판정 속도", ClampMin = "0.0"))
	float MinimumMoveSpeed = 3.0f;
};
