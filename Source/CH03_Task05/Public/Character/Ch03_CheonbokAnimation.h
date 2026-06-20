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

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|References")
	TObjectPtr<ACh03_CheonbokCharacter> CheonbokCharacter;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|References")
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|Movement")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|Movement")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|Movement")
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|Movement")
	float VerticalSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|Movement")
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|Movement")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Cheonbok|State")
	bool bIsDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|Settings",
		meta = (ClampMin = "0.0"))
	float MinimumMoveSpeed = 3.0f;
};
