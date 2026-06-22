#include "Character/Ch03_CheonbokCharacter.h"

#include "Camera/CameraComponent.h"
#include "Core/Ch03_CheonbokController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "UI/Ch03_WorldHealthWidget.h"
#include "Components/WidgetComponent.h"

ACh03_CheonbokCharacter::ACh03_CheonbokCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

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
		FVector(0.0f, 0.0f, 120.0f));
	WorldHealthWidgetComponent->SetDrawSize(FVector2D(220.0f, 52.0f));
	WorldHealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WorldHealthWidgetComponent->SetCollisionEnabled(
		ECollisionEnabled::NoCollision);
	WorldHealthWidgetComponent->SetGenerateOverlapEvents(false);
	WorldHealthWidgetComponent->SetWidgetClass(
		UCh03_WorldHealthWidget::StaticClass());
	WorldHealthWidgetClass = UCh03_WorldHealthWidget::StaticClass();

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
	InitializeWorldHealthWidget();
	RefreshMovementSpeed();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void ACh03_CheonbokCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if (!Controller || bIsDead || IsAirMovementLocked())
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
	if (!bIsDead && Value.Get<bool>() && CanJump())
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

void ACh03_CheonbokCharacter::ResetCharacterState()
{
	GetWorldTimerManager().ClearTimer(DamageInvincibilityTimerHandle);
	bIsDamageInvincible = false;
	bIsDead = false;
	bIsAirMovementLocked = false;

	ClearAllStatusEffects();
	CurrentHealth = MaxHealth;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
	}

	RefreshMovementSpeed();
	InitializeWorldHealthWidget();
	UpdateWorldHealthWidget();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
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

void ACh03_CheonbokCharacter::RefreshMovementSpeed()
{
	UCharacterMovementComponent* MovementComponent =
		GetCharacterMovement();
	if (!MovementComponent || bIsDead || IsAirMovementLocked())
	{
		return;
	}

	const float SprintMultiplier =
		IsSprintInputHeld() ? SprintSpeedMultiplier : 1.0f;
	const float StatusMultiplier =
		bIsSlowed ? ActiveSlowMultiplier : 1.0f;

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
