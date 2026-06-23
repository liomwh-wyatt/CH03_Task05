#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Ch03_CheonbokCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCh03_WorldHealthWidget;
class UWidgetComponent;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECheonbokStatusEffect : uint8
{
	Slow UMETA(DisplayName = "Slow"),
	ReverseControl UMETA(DisplayName = "Reverse Control"),
	MovementLock UMETA(DisplayName = "Movement Lock"),
	DamageShield UMETA(DisplayName = "Damage Shield")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCheonbokHealthChanged,
	float, CurrentHealth,
	float, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCheonbokStaminaChanged,
	float, CurrentStamina,
	float, MaxStamina);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheonbokDeath);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnCheonbokStatusEffectChanged,
	ECheonbokStatusEffect, EffectType,
	bool, bIsActive,
	int32, StackCount,
	float, RemainingTime);

UCLASS()
class CH03_TASK05_API ACh03_CheonbokCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACh03_CheonbokCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void PawnClientRestart() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Health")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Health")
	void AddHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Health")
	virtual void OnDeath();

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Stamina")
	float GetStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Stamina")
	float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Stamina")
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Stamina")
	void AddStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Status Effect")
	void ApplySlow(float Duration = 5.0f, float SpeedMultiplier = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Status Effect")
	void ApplyReverseControl(float Duration = 4.0f);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Status Effect")
	void ApplyMovementLock(float Duration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Shield")
	void ApplyDamageShield(int32 StackAmount = 1);

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|Status Effect")
	void ClearAllStatusEffects();

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	bool IsSlowActive() const { return bIsSlowed; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	bool IsReverseControlActive() const { return bIsControlReversed; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	bool IsMovementLockActive() const { return bIsMovementLocked; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Shield")
	bool IsDamageShieldActive() const { return DamageShieldStackCount > 0; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	int32 GetSlowStackCount() const { return SlowStackCount; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	int32 GetReverseControlStackCount() const { return ReverseControlStackCount; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	int32 GetMovementLockStackCount() const { return MovementLockStackCount; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Shield")
	int32 GetDamageShieldStackCount() const { return DamageShieldStackCount; }

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Shield")
	bool CanAddDamageShield(int32 StackAmount = 1) const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	float GetSlowRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	float GetReverseControlRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "Cheonbok|Status Effect")
	float GetMovementLockRemainingTime() const;

	UFUNCTION(BlueprintCallable, Category = "Cheonbok|State")
	void ResetCharacterState();

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokDeath OnCharacterDeath;

	UPROPERTY(BlueprintAssignable, Category = "Cheonbok|Events")
	FOnCheonbokStatusEffectChanged OnStatusEffectChanged;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);

	bool IsAirMovementLocked() const;
	bool IsSprintInputHeld() const;
	bool CanUseSprintSpeed() const;
	bool IsMovingOnGround() const;
	void UpdateStamina(float DeltaTime);
	void RefreshMovementSpeed();
	void ClampHorizontalVelocityToMaxSpeed();
	void EndSlow();
	void EndReverseControl();
	void EndMovementLock();
	void EndDamageInvincibility();
	void InitializeWorldHealthWidget();
	void UpdateWorldHealthWidget();

	float ExtendEffectTimer(
		FTimerHandle& TimerHandle,
		float AddedDuration,
		float MaximumDuration,
		FTimerDelegate EndDelegate);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|UI")
	TObjectPtr<UWidgetComponent> WorldHealthWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheonbok|UI")
	TSubclassOf<UCh03_WorldHealthWidget> WorldHealthWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|UI")
	bool bShowWorldHealthWidget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Camera",
		meta = (ClampMin = "-89.0", ClampMax = "0.0"))
	float InitialCameraPitch = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Input",
		meta = (ClampMin = "0.01"))
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Movement",
		meta = (ClampMin = "0.0"))
	float NormalSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Movement",
		meta = (ClampMin = "1.0"))
	float SprintSpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Stamina",
		meta = (ClampMin = "1.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Stamina")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Stamina",
		meta = (ClampMin = "0.0"))
	float SprintStaminaCostPerSecond = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Stamina",
		meta = (ClampMin = "0.0"))
	float StaminaRecoveryPerSecond = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Stamina",
		meta = (ClampMin = "0.0"))
	float MinStaminaToStartSprint = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Health",
		meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheonbok|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Health",
		meta = (ClampMin = "0.0"))
	float DamageInvincibilityDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Status Effect",
		meta = (ClampMin = "0.1"))
	float MaximumSlowDuration = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Status Effect",
		meta = (ClampMin = "0.1"))
	float MaximumReverseControlDuration = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Status Effect",
		meta = (ClampMin = "0.1"))
	float MaximumMovementLockDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cheonbok|Shield",
		meta = (ClampMin = "1"))
	int32 MaxDamageShieldStacks = 1;

private:
	bool bIsDead = false;
	bool bIsDamageInvincible = false;
	bool bIsSlowed = false;
	bool bIsControlReversed = false;
	bool bIsMovementLocked = false;
	bool bIsAirMovementLocked = false;
	bool bIsSprinting = false;

	float ActiveSlowMultiplier = 1.0f;
	int32 SlowStackCount = 0;
	int32 ReverseControlStackCount = 0;
	int32 MovementLockStackCount = 0;
	int32 DamageShieldStackCount = 0;

	FTimerHandle DamageInvincibilityTimerHandle;
	FTimerHandle SlowTimerHandle;
	FTimerHandle ReverseControlTimerHandle;
	FTimerHandle MovementLockTimerHandle;
};
