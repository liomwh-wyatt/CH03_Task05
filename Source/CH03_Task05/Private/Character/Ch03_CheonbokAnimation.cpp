// Copyright 2026 liomwh-wyatt. All Rights Reserved.

#include "Character/Ch03_CheonbokAnimation.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCh03_CheonbokAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheCharacterReferences();
}

void UCh03_CheonbokAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(CheonbokCharacter) || !IsValid(CharacterMovement))
	{
		CacheCharacterReferences();
	}

	if (!IsValid(CheonbokCharacter) || !IsValid(CharacterMovement))
	{
		ResetAnimationState();
		return;
	}

	Velocity = CheonbokCharacter->GetVelocity();
	Acceleration = CharacterMovement->GetCurrentAcceleration();

	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	VerticalSpeed = Velocity.Z;

	bShouldMove =
		GroundSpeed > MinimumMoveSpeed &&
		!Acceleration.IsNearlyZero();

	bIsFalling = CharacterMovement->IsFalling();
	bIsDead = CheonbokCharacter->IsDead();
}

void UCh03_CheonbokAnimation::CacheCharacterReferences()
{
	CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(TryGetPawnOwner());

	CharacterMovement = IsValid(CheonbokCharacter)
		? CheonbokCharacter->GetCharacterMovement()
		: nullptr;
}

void UCh03_CheonbokAnimation::ResetAnimationState()
{
	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	VerticalSpeed = 0.0f;
	bShouldMove = false;
	bIsFalling = false;
	bIsDead = false;
}
