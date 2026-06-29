#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Feedback/Ch03_FeedbackCue.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Ch03_CheonbokCharacter.generated.h"

class UCameraComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USceneCaptureComponent2D;
class USpringArmComponent;
class UCh03_WorldHealthWidget;
class UTextureRenderTarget2D;
class UWidgetComponent;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECheonbokStatusEffect : uint8
{
	Slow UMETA(DisplayName = "감속"),
	ReverseControl UMETA(DisplayName = "방향 반전"),
	MovementLock UMETA(DisplayName = "이동 잠금"),
	DamageShield UMETA(DisplayName = "피해 실드")
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
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PawnClientRestart() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "천복|체력")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "천복|체력")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "천복|체력")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "천복|체력")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "천복|체력")
	void AddHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "천복|체력")
	virtual void OnDeath();

	UFUNCTION(BlueprintPure, Category = "천복|스태미나")
	float GetStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "천복|스태미나")
	float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintPure, Category = "천복|스태미나")
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintCallable, Category = "천복|스태미나")
	void AddStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "천복|상태 효과")
	void ApplySlow(float Duration = 5.0f, float SpeedMultiplier = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "천복|상태 효과")
	void ApplyReverseControl(float Duration = 4.0f);

	UFUNCTION(BlueprintCallable, Category = "천복|상태 효과")
	void ApplyMovementLock(float Duration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "천복|이동")
	void ApplyKnockback(
		FVector Direction,
		float HorizontalStrength,
		float VerticalStrength = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "천복|보호막")
	void ApplyDamageShield(int32 StackAmount = 1);

	UFUNCTION(BlueprintCallable, Category = "천복|상태 효과")
	void ClearAllStatusEffects();

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	bool IsSlowActive() const { return bIsSlowed; }

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	bool IsReverseControlActive() const { return bIsControlReversed; }

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	bool IsMovementLockActive() const { return bIsMovementLocked; }

	UFUNCTION(BlueprintPure, Category = "천복|보호막")
	bool IsDamageShieldActive() const { return DamageShieldStackCount > 0; }

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	int32 GetSlowStackCount() const { return SlowStackCount; }

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	int32 GetReverseControlStackCount() const { return ReverseControlStackCount; }

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	int32 GetMovementLockStackCount() const { return MovementLockStackCount; }

	UFUNCTION(BlueprintPure, Category = "천복|보호막")
	int32 GetDamageShieldStackCount() const { return DamageShieldStackCount; }

	UFUNCTION(BlueprintPure, Category = "천복|보호막")
	bool CanAddDamageShield(int32 StackAmount = 1) const;

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	float GetSlowRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	float GetMaximumSlowDuration() const { return MaximumSlowDuration; }

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	float GetReverseControlRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	float GetMaximumReverseControlDuration() const
	{
		return MaximumReverseControlDuration;
	}

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	float GetMovementLockRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "천복|상태 효과")
	float GetMaximumMovementLockDuration() const
	{
		return MaximumMovementLockDuration;
	}

	UFUNCTION(BlueprintCallable, Category = "천복|상태")
	void ResetCharacterState();

	UFUNCTION(BlueprintCallable, Category = "천복|피드백")
	bool PlayScorePickupFeedback(
		AActor* SourceItem,
		int32 BaseScore,
		int32 FinalScore);

	UFUNCTION(BlueprintPure, Category = "천복|초상화")
	UTextureRenderTarget2D* GetPortraitRenderTarget() const
	{
		return PortraitRenderTarget;
	}

	UFUNCTION(BlueprintCallable, Category = "천복|초상화")
	void RefreshPortraitCapture() const;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokDeath OnCharacterDeath;

	UPROPERTY(BlueprintAssignable, Category = "천복|이벤트")
	FOnCheonbokStatusEffectChanged OnStatusEffectChanged;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnDamageTakenFeedback(float AppliedDamage, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnDamageBlockedFeedback(float BlockedDamage, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnHealthRecoveredFeedback(float RecoveredAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnStaminaRecoveredFeedback(float RecoveredAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnScorePickupFeedback(
		AActor* SourceItem,
		int32 BaseScore,
		int32 FinalScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnStatusEffectAppliedFeedback(
		ECheonbokStatusEffect EffectType,
		int32 StackCount,
		float RemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "천복|피드백")
	void OnStatusEffectEndedFeedback(ECheonbokStatusEffect EffectType);

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
	void ApplyMovementTuningSettings();
	void ApplyCameraSettings();
	void ClampHorizontalVelocityToMaxSpeed();
	void EndSlow();
	void EndReverseControl();
	void EndMovementLock();
	void EndDamageInvincibility();
	void InitializeWorldHealthWidget();
	void UpdateWorldHealthWidget();
	void ApplyPortraitCaptureSettings();
	void ApplyPortraitCaptureShowFlags();
	void ConfigurePortraitShowOnlyComponents();
	void ApplyMovementTrailSettings();
	void UpdateMovementTrail();
	bool ShouldPlayMovementTrail() const;
	void ApplyReverseControlFeedbackSettings();
	void StartReverseControlFeedbackEffect();
	void StopReverseControlFeedbackEffect();
	void PlayFeedbackCueSoundOnly(const FCh03FeedbackCue& FeedbackCue);

	float ExtendEffectTimer(
		FTimerHandle& TimerHandle,
		float AddedDuration,
		float MaximumDuration,
		FTimerDelegate EndDelegate);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|카메라", meta = (DisplayName = "스프링 암 컴포넌트"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|카메라", meta = (DisplayName = "카메라 컴포넌트"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "월드 체력바 컴포넌트"))
	TObjectPtr<UWidgetComponent> WorldHealthWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 캡처 컴포넌트"))
	TObjectPtr<USceneCaptureComponent2D> PortraitCaptureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "이동 궤적 컴포넌트"))
	TObjectPtr<UNiagaraComponent> MovementTrailComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "방향 반전 피드백 컴포넌트"))
	TObjectPtr<UNiagaraComponent> ReverseControlFeedbackComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "월드 체력바 상대 위치"))
	FVector WorldHealthWidgetRelativeLocation = FVector(0.0f, 0.0f, 86.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "월드 체력바 위젯 클래스"))
	TSubclassOf<UCh03_WorldHealthWidget> WorldHealthWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면", meta = (DisplayName = "월드 체력바 표시"))
	bool bShowWorldHealthWidget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 렌더 타겟"))
	TObjectPtr<UTextureRenderTarget2D> PortraitRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 캡처 상대 위치"))
	FVector PortraitCaptureRelativeLocation = FVector(130.0f, 0.0f, 62.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 캡처 상대 회전"))
	FRotator PortraitCaptureRelativeRotation = FRotator(0.0f, 180.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화",
		meta = (DisplayName = "초상화 캡처 시야각", ClampMin = "5.0", ClampMax = "120.0"))
	float PortraitCaptureFOV = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 캡처 소스"))
	TEnumAsByte<ESceneCaptureSource> PortraitCaptureSource = SCS_BaseColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 조명 영향 제거"))
	bool bUseUnlitPortraitCapture = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|화면|초상화", meta = (DisplayName = "초상화 매 프레임 캡처"))
	bool bCapturePortraitEveryFrame = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라",
		meta = (DisplayName = "초기 카메라 피치", ClampMin = "-89.0", ClampMax = "0.0"))
	float InitialCameraPitch = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라",
		meta = (DisplayName = "카메라 거리", ClampMin = "100.0", Units = "cm"))
	float CameraArmLength = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라",
		meta = (DisplayName = "카메라 시야각", ClampMin = "30.0", ClampMax = "120.0", Units = "deg"))
	float CameraFieldOfView = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라", meta = (DisplayName = "카메라 지연 사용"))
	bool bUseCameraLag = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라",
		meta = (DisplayName = "카메라 지연 속도", ClampMin = "0.0", EditCondition = "bUseCameraLag"))
	float CameraLagSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라", meta = (DisplayName = "카메라 회전 지연 사용"))
	bool bUseCameraRotationLag = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|카메라",
		meta = (DisplayName = "카메라 회전 지연 속도", ClampMin = "0.0", EditCondition = "bUseCameraRotationLag"))
	float CameraRotationLagSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "이동 궤적 이펙트"))
	TObjectPtr<UNiagaraSystem> MovementTrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "이동 궤적 사용"))
	bool bEnableMovementTrail = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "달리기 중에만 이동 궤적"))
	bool bMovementTrailRequiresSprint = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "이동 궤적 상대 위치"))
	FVector MovementTrailRelativeLocation = FVector(-35.0f, 0.0f, -82.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이펙트", meta = (DisplayName = "이동 궤적 상대 회전"))
	FRotator MovementTrailRelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이펙트",
		meta = (DisplayName = "이동 궤적 활성 속도", ClampMin = "0.0", Units = "cm/s"))
	float MovementTrailActivationSpeed = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|입력",
		meta = (DisplayName = "마우스 감도", ClampMin = "0.01"))
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이동",
		meta = (DisplayName = "기본 이동 속도", ClampMin = "0.0"))
	float NormalSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이동",
		meta = (DisplayName = "지상 가속도", ClampMin = "0.0", Units = "cm/s^2"))
	float GroundAcceleration = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이동",
		meta = (DisplayName = "지상 감속도", ClampMin = "0.0", Units = "cm/s^2"))
	float GroundBrakingDeceleration = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이동",
		meta = (DisplayName = "지상 마찰", ClampMin = "0.0"))
	float GroundFriction = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|이동",
		meta = (DisplayName = "달리기 속도 배율", ClampMin = "1.0"))
	float SprintSpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|스태미나",
		meta = (DisplayName = "최대 스태미나", ClampMin = "1.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|스태미나", meta = (DisplayName = "현재 스태미나"))
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|스태미나",
		meta = (DisplayName = "초당 달리기 스태미나 소모", ClampMin = "0.0"))
	float SprintStaminaCostPerSecond = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|스태미나",
		meta = (DisplayName = "초당 스태미나 회복", ClampMin = "0.0"))
	float StaminaRecoveryPerSecond = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|스태미나",
		meta = (DisplayName = "달리기 시작 최소 스태미나", ClampMin = "0.0"))
	float MinStaminaToStartSprint = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|체력",
		meta = (DisplayName = "최대 체력", ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "천복|체력", meta = (DisplayName = "현재 체력"))
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|체력",
		meta = (DisplayName = "피해 후 무적 시간", ClampMin = "0.0"))
	float DamageInvincibilityDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|상태 효과",
		meta = (DisplayName = "최대 감속 지속시간", ClampMin = "0.1"))
	float MaximumSlowDuration = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|상태 효과",
		meta = (DisplayName = "최대 방향 반전 지속시간", ClampMin = "0.1"))
	float MaximumReverseControlDuration = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|상태 효과",
		meta = (DisplayName = "최대 이동 잠금 지속시간", ClampMin = "0.1"))
	float MaximumMovementLockDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "천복|보호막",
		meta = (DisplayName = "최대 피해 실드 수", ClampMin = "1"))
	int32 MaxDamageShieldStacks = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "점수 획득 피드백", ToolTip = "점수 아이템을 먹었을 때 천복이에게 재생할 공통 피드백입니다. 점수량이 달라도 같은 피드백을 사용할 수 있습니다."))
	FCh03FeedbackCue ScorePickupFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "피해 피드백", ToolTip = "천복이가 실제 피해를 받았을 때 재생합니다. 무적 상태나 실드로 막힌 피해에는 사용하지 않습니다."))
	FCh03FeedbackCue DamageTakenFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "피해 차단 피드백", ToolTip = "피해 실드가 공격을 막았을 때 재생합니다. 실드 스택이 줄어드는 순간에 사용합니다."))
	FCh03FeedbackCue DamageBlockedFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "체력 회복 피드백", ToolTip = "천복이의 체력이 회복됐을 때 재생합니다."))
	FCh03FeedbackCue HealthRecoveredFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "스태미나 회복 피드백", ToolTip = "천복이의 스태미나가 회복됐을 때 재생합니다."))
	FCh03FeedbackCue StaminaRecoveredFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "감속 적용 피드백", ToolTip = "감속 디버프가 적용될 때 한 번 재생합니다."))
	FCh03FeedbackCue SlowAppliedFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "방향 반전 적용 피드백", ToolTip = "방향 반전 디버프용 피드백입니다. 사운드는 적용 순간 한 번 재생하고, 이펙트는 상태가 유지되는 동안 천복이에게 붙여 둡니다."))
	FCh03FeedbackCue ReverseControlAppliedFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "이동 잠금 적용 피드백", ToolTip = "이동 잠금 디버프가 적용될 때 한 번 재생합니다."))
	FCh03FeedbackCue MovementLockAppliedFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "피해 실드 적용 피드백", ToolTip = "피해 실드가 적용될 때 재생합니다. 실드 스택이 추가되는 순간에 사용합니다."))
	FCh03FeedbackCue DamageShieldAppliedFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "천복|피드백",
		meta = (DisplayName = "상태 효과 종료 피드백", ToolTip = "상태 효과가 끝났을 때 재생할 공통 피드백입니다."))
	FCh03FeedbackCue StatusEffectEndedFeedback;

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
