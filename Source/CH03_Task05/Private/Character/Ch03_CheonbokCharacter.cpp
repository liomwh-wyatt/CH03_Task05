#include "Character/Ch03_CheonbokCharacter.h"

#include "Camera/CameraComponent.h"
#include "Core/Ch03_CheonbokController.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "UI/Ch03_WorldHealthWidget.h"
#include "Components/WidgetComponent.h"

ACh03_CheonbokCharacter::ACh03_CheonbokCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 800.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 8.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(
		TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(
		SpringArmComponent,
		USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	WorldHealthWidgetComponent =
		CreateDefaultSubobject<UWidgetComponent>(
			TEXT("WorldHealthWidgetComponent"));
	WorldHealthWidgetComponent->SetupAttachment(RootComponent);
	WorldHealthWidgetComponent->SetRelativeLocation(
		WorldHealthWidgetRelativeLocation);
	WorldHealthWidgetComponent->SetDrawSize(FVector2D(220.0f, 52.0f));
	WorldHealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WorldHealthWidgetComponent->SetCollisionEnabled(
		ECollisionEnabled::NoCollision);
	WorldHealthWidgetComponent->SetGenerateOverlapEvents(false);
	WorldHealthWidgetComponent->SetWidgetClass(
		UCh03_WorldHealthWidget::StaticClass());
	WorldHealthWidgetClass = UCh03_WorldHealthWidget::StaticClass();

	PortraitCaptureComponent =
		CreateDefaultSubobject<USceneCaptureComponent2D>(
			TEXT("PortraitCaptureComponent"));
	PortraitCaptureComponent->SetupAttachment(RootComponent);
	PortraitCaptureComponent->SetRelativeLocation(
		PortraitCaptureRelativeLocation);
	PortraitCaptureComponent->SetRelativeRotation(
		PortraitCaptureRelativeRotation);
	PortraitCaptureComponent->FOVAngle = PortraitCaptureFOV;
	PortraitCaptureComponent->bCaptureEveryFrame = bCapturePortraitEveryFrame;
	PortraitCaptureComponent->bCaptureOnMovement = true;
	PortraitCaptureComponent->CaptureSource = PortraitCaptureSource;
	PortraitCaptureComponent->PrimitiveRenderMode =
		ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		MovementComponent->JumpZVelocity = 500.0f;
		MovementComponent->AirControl = 0.0f;
		MovementComponent->FallingLateralFriction = 0.0f;
		MovementComponent->BrakingDecelerationFalling = 0.0f;
		MovementComponent->MaxWalkSpeed = NormalSpeed;
	}

	Tags.AddUnique(TEXT("Player"));
}

void ACh03_CheonbokCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	ApplyPortraitCaptureSettings();
	InitializeWorldHealthWidget();
	RefreshMovementSpeed();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void ACh03_CheonbokCharacter::OnConstruction(
	const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyPortraitCaptureSettings();
}

void ACh03_CheonbokCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStamina(DeltaTime);
	RefreshMovementSpeed();
}

void ACh03_CheonbokCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (Controller)
	{
		const FRotator CurrentControlRotation = Controller->GetControlRotation();
		Controller->SetControlRotation(
			FRotator(InitialCameraPitch, CurrentControlRotation.Yaw, 0.0f));
	}
}

void ACh03_CheonbokCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bIsAirMovementLocked = false;
	RefreshMovementSpeed();
	ClampHorizontalVelocityToMaxSpeed();
}

void ACh03_CheonbokCharacter::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInputComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CheonbokCharacter requires an Enhanced Input Component."));
		return;
	}

	const ACh03_CheonbokController* CheonbokController =
		Cast<ACh03_CheonbokController>(GetController());
	if (!CheonbokController)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CheonbokCharacter requires Ch03_CheonbokController."));
		return;
	}

	if (CheonbokController->MoveAction)
	{
		EnhancedInputComponent->BindAction(
			CheonbokController->MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ACh03_CheonbokCharacter::Move);
	}

	if (CheonbokController->LookAction)
	{
		EnhancedInputComponent->BindAction(
			CheonbokController->LookAction,
			ETriggerEvent::Triggered,
			this,
			&ACh03_CheonbokCharacter::Look);
	}

	if (CheonbokController->JumpAction)
	{
		EnhancedInputComponent->BindAction(
			CheonbokController->JumpAction,
			ETriggerEvent::Started,
			this,
			&ACh03_CheonbokCharacter::StartJump);
		EnhancedInputComponent->BindAction(
			CheonbokController->JumpAction,
			ETriggerEvent::Completed,
			this,
			&ACh03_CheonbokCharacter::StopJump);
	}

	if (CheonbokController->SprintAction)
	{
		EnhancedInputComponent->BindAction(
			CheonbokController->SprintAction,
			ETriggerEvent::Started,
			this,
			&ACh03_CheonbokCharacter::StartSprint);
		EnhancedInputComponent->BindAction(
			CheonbokController->SprintAction,
			ETriggerEvent::Completed,
			this,
			&ACh03_CheonbokCharacter::StopSprint);
		EnhancedInputComponent->BindAction(
			CheonbokController->SprintAction,
			ETriggerEvent::Canceled,
			this,
			&ACh03_CheonbokCharacter::StopSprint);
	}
}

void ACh03_CheonbokCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller || bIsDead || bIsMovementLocked || IsAirMovementLocked())
	{
		return;
	}

	FVector2D MoveInput = Value.Get<FVector2D>();

	if (bIsControlReversed)
	{
		MoveInput *= -1.0f;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(ForwardDirection, MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(RightDirection, MoveInput.Y);
	}
}

void ACh03_CheonbokCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller || bIsDead)
	{
		return;
	}

	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X * MouseSensitivity);
	AddControllerPitchInput(LookInput.Y * MouseSensitivity);
}

void ACh03_CheonbokCharacter::StartJump(const FInputActionValue& Value)
{
	if (!bIsDead && !bIsMovementLocked && Value.Get<bool>() && CanJump())
	{
		bIsAirMovementLocked = true;
		Jump();
	}
}

void ACh03_CheonbokCharacter::StopJump(const FInputActionValue& Value)
{
	(void)Value;
	StopJumping();
}

void ACh03_CheonbokCharacter::StartSprint(const FInputActionValue& Value)
{
	(void)Value;

	if (bIsDead || IsAirMovementLocked())
	{
		return;
	}

	RefreshMovementSpeed();
}

void ACh03_CheonbokCharacter::StopSprint(const FInputActionValue& Value)
{
	(void)Value;

	bIsSprinting = false;

	if (!IsAirMovementLocked())
	{
		RefreshMovementSpeed();
		ClampHorizontalVelocityToMaxSpeed();
	}
}

float ACh03_CheonbokCharacter::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

float ACh03_CheonbokCharacter::GetStaminaPercent() const
{
	return MaxStamina > 0.0f
		? FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f)
		: 0.0f;
}

void ACh03_CheonbokCharacter::AddHealth(float Amount)
{
	if (Amount <= 0.0f || bIsDead)
	{
		return;
	}

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(
		CurrentHealth + Amount,
		0.0f,
		MaxHealth);

	if (!FMath::IsNearlyEqual(PreviousHealth, CurrentHealth))
	{
		UpdateWorldHealthWidget();
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
		UE_LOG(
			LogTemp,
			Log,
			TEXT("Cheonbok health increased: %.1f / %.1f"),
			CurrentHealth,
			MaxHealth);
	}
}

void ACh03_CheonbokCharacter::AddStamina(float Amount)
{
	if (Amount <= 0.0f || bIsDead)
	{
		return;
	}

	const float PreviousStamina = CurrentStamina;
	CurrentStamina = FMath::Clamp(
		CurrentStamina + Amount,
		0.0f,
		MaxStamina);

	if (!FMath::IsNearlyEqual(PreviousStamina, CurrentStamina))
	{
		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
		RefreshMovementSpeed();

		UE_LOG(
			LogTemp,
			Log,
			TEXT("Cheonbok stamina increased: %.1f / %.1f"),
			CurrentStamina,
			MaxStamina);
	}
}

float ACh03_CheonbokCharacter::TakeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || bIsDead || bIsDamageInvincible)
	{
		return 0.0f;
	}

	if (DamageShieldStackCount > 0)
	{
		--DamageShieldStackCount;
		OnStatusEffectChanged.Broadcast(
			ECheonbokStatusEffect::DamageShield,
			DamageShieldStackCount > 0,
			DamageShieldStackCount,
			-1.0f);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("Cheonbok damage shield blocked %.1f damage. Remaining shield: %d"),
			DamageAmount,
			DamageShieldStackCount);

		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser);

	if (ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(
		CurrentHealth - ActualDamage,
		0.0f,
		MaxHealth);
	const float AppliedDamage = PreviousHealth - CurrentHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	UpdateWorldHealthWidget();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Cheonbok took %.1f damage: %.1f / %.1f"),
		AppliedDamage,
		CurrentHealth,
		MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}
	else if (DamageInvincibilityDuration > 0.0f)
	{
		bIsDamageInvincible = true;
		GetWorldTimerManager().SetTimer(
			DamageInvincibilityTimerHandle,
			this,
			&ACh03_CheonbokCharacter::EndDamageInvincibility,
			DamageInvincibilityDuration,
			false);
	}

	return AppliedDamage;
}

void ACh03_CheonbokCharacter::OnDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	StopJumping();
	ClearAllStatusEffects();
	DamageShieldStackCount = 0;
	UpdateWorldHealthWidget();

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->DisableMovement();
	}

	OnCharacterDeath.Broadcast();
	UE_LOG(LogTemp, Error, TEXT("Cheonbok is dead."));
}

void ACh03_CheonbokCharacter::ApplySlow(
	float Duration,
	float SpeedMultiplier)
{
	if (Duration <= 0.0f || bIsDead)
	{
		return;
	}

	bIsSlowed = true;
	ActiveSlowMultiplier = FMath::Clamp(
		SpeedMultiplier,
		0.1f,
		1.0f);
	SlowStackCount = FMath::Max(1, SlowStackCount + 1);

	const float NewRemainingTime = ExtendEffectTimer(
		SlowTimerHandle,
		Duration,
		MaximumSlowDuration,
		FTimerDelegate::CreateUObject(
			this,
			&ACh03_CheonbokCharacter::EndSlow));

	RefreshMovementSpeed();
	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::Slow,
		true,
		SlowStackCount,
		NewRemainingTime);
}

void ACh03_CheonbokCharacter::ApplyReverseControl(float Duration)
{
	if (Duration <= 0.0f || bIsDead)
	{
		return;
	}

	bIsControlReversed = true;
	ReverseControlStackCount =
		FMath::Max(1, ReverseControlStackCount + 1);

	const float NewRemainingTime = ExtendEffectTimer(
		ReverseControlTimerHandle,
		Duration,
		MaximumReverseControlDuration,
		FTimerDelegate::CreateUObject(
			this,
			&ACh03_CheonbokCharacter::EndReverseControl));

	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::ReverseControl,
		true,
		ReverseControlStackCount,
		NewRemainingTime);
}

void ACh03_CheonbokCharacter::ApplyMovementLock(float Duration)
{
	if (Duration <= 0.0f || bIsDead)
	{
		return;
	}

	bIsMovementLocked = true;
	bIsSprinting = false;
	MovementLockStackCount = FMath::Max(1, MovementLockStackCount + 1);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}

	const float NewRemainingTime = ExtendEffectTimer(
		MovementLockTimerHandle,
		Duration,
		MaximumMovementLockDuration,
		FTimerDelegate::CreateUObject(
			this,
			&ACh03_CheonbokCharacter::EndMovementLock));

	RefreshMovementSpeed();
	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::MovementLock,
		true,
		MovementLockStackCount,
		NewRemainingTime);
}

void ACh03_CheonbokCharacter::ApplyKnockback(
	FVector Direction,
	const float HorizontalStrength,
	const float VerticalStrength)
{
	if (bIsDead
		|| (HorizontalStrength <= 0.0f && VerticalStrength <= 0.0f))
	{
		return;
	}

	Direction.Z = 0.0f;
	FVector KnockbackDirection = Direction.GetSafeNormal();
	if (KnockbackDirection.IsNearlyZero())
	{
		KnockbackDirection = -GetActorForwardVector().GetSafeNormal2D();
	}

	const FVector LaunchVelocity =
		KnockbackDirection * FMath::Max(0.0f, HorizontalStrength)
		+ FVector::UpVector * FMath::Max(0.0f, VerticalStrength);

	bIsSprinting = false;
	bIsAirMovementLocked = true;

	LaunchCharacter(
		LaunchVelocity,
		true,
		VerticalStrength > 0.0f);

	RefreshMovementSpeed();
}

void ACh03_CheonbokCharacter::ApplyDamageShield(const int32 StackAmount)
{
	if (StackAmount <= 0 || bIsDead)
	{
		return;
	}

	DamageShieldStackCount = FMath::Clamp(
		DamageShieldStackCount + StackAmount,
		0,
		FMath::Max(1, MaxDamageShieldStacks));

	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::DamageShield,
		DamageShieldStackCount > 0,
		DamageShieldStackCount,
		-1.0f);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Cheonbok damage shield applied. Shield stack: %d"),
		DamageShieldStackCount);
}

void ACh03_CheonbokCharacter::ClearAllStatusEffects()
{
	if (bIsSlowed)
	{
		EndSlow();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SlowTimerHandle);
	}

	if (bIsControlReversed)
	{
		EndReverseControl();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ReverseControlTimerHandle);
	}

	if (bIsMovementLocked)
	{
		EndMovementLock();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(MovementLockTimerHandle);
	}
}

float ACh03_CheonbokCharacter::GetSlowRemainingTime() const
{
	return GetWorld()
		? FMath::Max(
			0.0f,
			GetWorld()->GetTimerManager().GetTimerRemaining(
				SlowTimerHandle))
		: 0.0f;
}

float ACh03_CheonbokCharacter::GetReverseControlRemainingTime() const
{
	return GetWorld()
		? FMath::Max(
			0.0f,
			GetWorld()->GetTimerManager().GetTimerRemaining(
				ReverseControlTimerHandle))
		: 0.0f;
}

float ACh03_CheonbokCharacter::GetMovementLockRemainingTime() const
{
	return GetWorld()
		? FMath::Max(
			0.0f,
			GetWorld()->GetTimerManager().GetTimerRemaining(
				MovementLockTimerHandle))
		: 0.0f;
}

bool ACh03_CheonbokCharacter::CanAddDamageShield(
	const int32 StackAmount) const
{
	return StackAmount > 0
		&& !bIsDead
		&& DamageShieldStackCount < FMath::Max(1, MaxDamageShieldStacks);
}

void ACh03_CheonbokCharacter::ResetCharacterState()
{
	GetWorldTimerManager().ClearTimer(DamageInvincibilityTimerHandle);
	bIsDamageInvincible = false;
	bIsDead = false;
	bIsSprinting = false;
	bIsAirMovementLocked = false;
	DamageShieldStackCount = 0;

	ClearAllStatusEffects();
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
	}

	RefreshMovementSpeed();
	InitializeWorldHealthWidget();
	UpdateWorldHealthWidget();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::DamageShield,
		false,
		0,
		-1.0f);
}

void ACh03_CheonbokCharacter::RefreshPortraitCapture() const
{
	if (PortraitCaptureComponent && PortraitRenderTarget)
	{
		PortraitCaptureComponent->CaptureScene();
	}
}

bool ACh03_CheonbokCharacter::IsAirMovementLocked() const
{
	if (bIsAirMovementLocked)
	{
		return true;
	}

	const UCharacterMovementComponent* MovementComponent =
		GetCharacterMovement();

	return MovementComponent && MovementComponent->IsFalling();
}

bool ACh03_CheonbokCharacter::IsSprintInputHeld() const
{
	const ACh03_CheonbokController* CheonbokController =
		Cast<ACh03_CheonbokController>(GetController());

	return CheonbokController
		&& CheonbokController->IsSprintInputHeld();
}

bool ACh03_CheonbokCharacter::CanUseSprintSpeed() const
{
	if (bIsDead
		|| bIsMovementLocked
		|| IsAirMovementLocked()
		|| !IsSprintInputHeld())
	{
		return false;
	}

	const float RequiredStamina = bIsSprinting
		? KINDA_SMALL_NUMBER
		: MinStaminaToStartSprint;

	return CurrentStamina >= RequiredStamina;
}

bool ACh03_CheonbokCharacter::IsMovingOnGround() const
{
	const UCharacterMovementComponent* MovementComponent =
		GetCharacterMovement();

	return MovementComponent
		&& MovementComponent->IsMovingOnGround()
		&& MovementComponent->GetCurrentAcceleration().SizeSquared2D()
			> KINDA_SMALL_NUMBER;
}

void ACh03_CheonbokCharacter::UpdateStamina(const float DeltaTime)
{
	if (bIsDead || DeltaTime <= 0.0f || MaxStamina <= 0.0f)
	{
		return;
	}

	const float PreviousStamina = CurrentStamina;
	const bool bShouldSpendStamina =
		CanUseSprintSpeed()
		&& IsMovingOnGround()
		&& SprintStaminaCostPerSecond > 0.0f;

	if (bShouldSpendStamina)
	{
		CurrentStamina = FMath::Clamp(
			CurrentStamina - SprintStaminaCostPerSecond * DeltaTime,
			0.0f,
			MaxStamina);

		if (CurrentStamina <= KINDA_SMALL_NUMBER)
		{
			CurrentStamina = 0.0f;
			bIsSprinting = false;
		}
	}
	else if (StaminaRecoveryPerSecond > 0.0f)
	{
		CurrentStamina = FMath::Clamp(
			CurrentStamina + StaminaRecoveryPerSecond * DeltaTime,
			0.0f,
			MaxStamina);
	}

	if (!FMath::IsNearlyEqual(PreviousStamina, CurrentStamina, 0.05f))
	{
		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	}
}

void ACh03_CheonbokCharacter::RefreshMovementSpeed()
{
	UCharacterMovementComponent* MovementComponent =
		GetCharacterMovement();
	if (!MovementComponent || bIsDead)
	{
		return;
	}

	if (bIsMovementLocked)
	{
		bIsSprinting = false;
		MovementComponent->MaxWalkSpeed = 0.0f;
		return;
	}

	if (IsAirMovementLocked())
	{
		return;
	}

	const float SprintMultiplier =
		CanUseSprintSpeed() ? SprintSpeedMultiplier : 1.0f;
	const float StatusMultiplier =
		bIsSlowed ? ActiveSlowMultiplier : 1.0f;

	bIsSprinting = SprintMultiplier > 1.0f;
	MovementComponent->MaxWalkSpeed =
		NormalSpeed * SprintMultiplier * StatusMultiplier;
}

void ACh03_CheonbokCharacter::ClampHorizontalVelocityToMaxSpeed()
{
	UCharacterMovementComponent* MovementComponent =
		GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	FVector HorizontalVelocity(
		MovementComponent->Velocity.X,
		MovementComponent->Velocity.Y,
		0.0f);

	HorizontalVelocity = HorizontalVelocity.GetClampedToMaxSize(
		MovementComponent->MaxWalkSpeed);

	MovementComponent->Velocity.X = HorizontalVelocity.X;
	MovementComponent->Velocity.Y = HorizontalVelocity.Y;
}

void ACh03_CheonbokCharacter::EndSlow()
{
	GetWorldTimerManager().ClearTimer(SlowTimerHandle);
	bIsSlowed = false;
	ActiveSlowMultiplier = 1.0f;
	SlowStackCount = 0;

	RefreshMovementSpeed();
	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::Slow,
		false,
		0,
		0.0f);
}

void ACh03_CheonbokCharacter::EndReverseControl()
{
	GetWorldTimerManager().ClearTimer(ReverseControlTimerHandle);
	bIsControlReversed = false;
	ReverseControlStackCount = 0;

	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::ReverseControl,
		false,
		0,
		0.0f);
}

void ACh03_CheonbokCharacter::EndMovementLock()
{
	GetWorldTimerManager().ClearTimer(MovementLockTimerHandle);
	bIsMovementLocked = false;
	MovementLockStackCount = 0;

	RefreshMovementSpeed();
	OnStatusEffectChanged.Broadcast(
		ECheonbokStatusEffect::MovementLock,
		false,
		0,
		0.0f);
}

void ACh03_CheonbokCharacter::EndDamageInvincibility()
{
	bIsDamageInvincible = false;
}

void ACh03_CheonbokCharacter::InitializeWorldHealthWidget()
{
	if (!WorldHealthWidgetComponent)
	{
		return;
	}

	if (WorldHealthWidgetClass)
	{
		WorldHealthWidgetComponent->SetWidgetClass(WorldHealthWidgetClass);
	}

	WorldHealthWidgetComponent->SetRelativeLocation(
		WorldHealthWidgetRelativeLocation);
	WorldHealthWidgetComponent->SetVisibility(
		bShowWorldHealthWidget && !bIsDead);
	WorldHealthWidgetComponent->InitWidget();

	UpdateWorldHealthWidget();
}

void ACh03_CheonbokCharacter::UpdateWorldHealthWidget()
{
	if (!WorldHealthWidgetComponent)
	{
		return;
	}

	WorldHealthWidgetComponent->SetVisibility(
		bShowWorldHealthWidget && !bIsDead);

	if (UCh03_WorldHealthWidget* WorldHealthWidget =
		Cast<UCh03_WorldHealthWidget>(
			WorldHealthWidgetComponent->GetUserWidgetObject()))
	{
		WorldHealthWidget->SetHealthValues(CurrentHealth, MaxHealth);
	}
}

void ACh03_CheonbokCharacter::ApplyPortraitCaptureSettings()
{
	if (!PortraitCaptureComponent)
	{
		return;
	}

	PortraitCaptureComponent->SetRelativeLocation(
		PortraitCaptureRelativeLocation);
	PortraitCaptureComponent->SetRelativeRotation(
		PortraitCaptureRelativeRotation);
	PortraitCaptureComponent->FOVAngle = PortraitCaptureFOV;
	PortraitCaptureComponent->TextureTarget = PortraitRenderTarget;
	PortraitCaptureComponent->CaptureSource = PortraitCaptureSource;
	PortraitCaptureComponent->bCaptureEveryFrame =
		bCapturePortraitEveryFrame && PortraitRenderTarget != nullptr;
	PortraitCaptureComponent->bCaptureOnMovement =
		PortraitRenderTarget != nullptr;
	PortraitCaptureComponent->PrimitiveRenderMode =
		ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

	ApplyPortraitCaptureShowFlags();
	ConfigurePortraitShowOnlyComponents();
}

void ACh03_CheonbokCharacter::ApplyPortraitCaptureShowFlags()
{
	if (!PortraitCaptureComponent || !bUseUnlitPortraitCapture)
	{
		return;
	}

	PortraitCaptureComponent->ShowFlags.DisableFeaturesForUnlit(false);
	PortraitCaptureComponent->ShowFlags.SetAmbientOcclusion(false);
	PortraitCaptureComponent->ShowFlags.SetAtmosphere(false);
	PortraitCaptureComponent->ShowFlags.SetCloud(false);
	PortraitCaptureComponent->ShowFlags.SetContactShadows(false);
	PortraitCaptureComponent->ShowFlags.SetDirectionalLights(false);
	PortraitCaptureComponent->ShowFlags.SetDynamicShadows(false);
	PortraitCaptureComponent->ShowFlags.SetFog(false);
	PortraitCaptureComponent->ShowFlags.SetLightShafts(false);
	PortraitCaptureComponent->ShowFlags.SetLighting(false);
	PortraitCaptureComponent->ShowFlags.SetPointLights(false);
	PortraitCaptureComponent->ShowFlags.SetPostProcessing(false);
	PortraitCaptureComponent->ShowFlags.SetSkyLighting(false);
	PortraitCaptureComponent->ShowFlags.SetSpotLights(false);
	PortraitCaptureComponent->ShowFlags.SetVolumetricFog(false);
}

void ACh03_CheonbokCharacter::ConfigurePortraitShowOnlyComponents()
{
	if (!PortraitCaptureComponent)
	{
		return;
	}

	PortraitCaptureComponent->ClearShowOnlyComponents();

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		PortraitCaptureComponent->ShowOnlyComponent(MeshComponent);
	}
}

float ACh03_CheonbokCharacter::ExtendEffectTimer(
	FTimerHandle& TimerHandle,
	float AddedDuration,
	float MaximumDuration,
	FTimerDelegate EndDelegate)
{
	float RemainingTime = 0.0f;
	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
	{
		RemainingTime = FMath::Max(
			0.0f,
			GetWorldTimerManager().GetTimerRemaining(TimerHandle));
	}

	const float NewDuration = FMath::Clamp(
		RemainingTime + AddedDuration,
		0.01f,
		MaximumDuration);

	GetWorldTimerManager().SetTimer(
		TimerHandle,
		MoveTemp(EndDelegate),
		NewDuration,
		false);

	return NewDuration;
}
