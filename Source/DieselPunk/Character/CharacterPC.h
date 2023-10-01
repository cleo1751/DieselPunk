// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CharacterBase.h"
#include "InputActionValue.h"
#include "DieselPunk/Skill/SkillManager.h"
#include "CharacterPC.generated.h"

class USkillBase;
enum class EAbilityType : uint8;

UCLASS(config = Game)
class ACharacterPC : public ACharacterBase
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent *CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *FollowCamera;


public:
	//TODO"현재 레벨에 미리 배치되어있는 액터는 데이터테이블 정보를 알지 못해서 일단 임시 처리 추후 관리방법 논의 필요"
	UPROPERTY( EditAnywhere, Category = "Test" )
	int32 TestInfoId;
	
	// 스킬을 할당받기 위한 배열입니다. (BP에서도 받을 수 있도록 TSubClassOf로 하였습니다)
	UPROPERTY(EditAnywhere, Category = Skill)
	TMap<EAbilityType, TSubclassOf<USkillBase>> SkillInfos;

	// 런타임에 인스턴스화된 스킬들을 담는 배열입니다. (실제로 이 배열에 담긴 스킬들을 호출)
	UPROPERTY()
	TMap<EAbilityType, TObjectPtr<USkillBase>> Skills;

public:
	ACharacterPC();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	// 상태 UI 위젯을 생성한다.
	virtual void CreateStatusUI() override;
	
	// 런타임에 스킬들을 인스턴스화 시킵니다. PlayerController에서 Pose시 호출합니다.
	void InitSkills();

	// 런타임에 인스턴스화 된 Skills Getter입니다.
	FORCEINLINE const TMap<EAbilityType, TObjectPtr<USkillBase>> &GetSkills() const { return Skills; }

	// PlayerController에서 호출될 스킬 발동 함수들입니다.
	void SkillStarted(const EAbilityType InAbilityType);
	void SkillOngoing(const EAbilityType InAbilityType);
	void SkillTriggered(const EAbilityType InAbilityType);
	void SkillCompleted(const EAbilityType InAbilityType);
	void SkillCanceled(const EAbilityType InAbilityType);

	SkillManager PCSkillManager;
public:
	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	/** Called for looking input */
	void Look(const FInputActionValue &Value);

	// !MyCode------------------------------------------------------
protected:
	// 로테이션 레이트 RotationRate
	float GetRotationRateVelocity();
	
	// Debug
	void DebugActorRotation();

public:
	// Zoom
	void SetZoomInProp();
	void SetZoomOutProp();
	void ZoomInOut(float DeltaTime);
	
	void SetRunZoomOutProp();

	void Jog();
	void WPressed();
	void WReleased();
	void SPressed();
	void SReleased();
	
	// RotatePawn
	void RotatePawn(float DeltaTime);

	// Movement
	void SetThisSpeed(float Speed);
	void SetThisJogSpeed(float JogSpeed);
	void CalculateSpeed();

	// TakeDamageHandle
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	
	void SetTakeDamageAnimFalse();
	void SetCanTakeDamageAnimTrue();
	FTimerHandle TakeDamageAnimHandle;

	// CombatState
	void HandleCombatState();
	void SetInCombatFalse();

	//=================================================================================

	// * 체력바UI
	UFUNCTION(BlueprintPure)
	float GetHealthPercent();
	
	UFUNCTION(BlueprintPure)
	float GetHealthPercentAfterImage();

	
	// 체력 퍼센테이지 반환 애니메이팅
	void _UpdateHp(int InCurHp, int InMaxHp);
	Animator AnimatorHealthPercent;
	float TempPercent;
	Animator AnimatorHealthPercentAfterImage;
	float TempPercentAfterImage;

	UFUNCTION(BlueprintPure)
	float GetSkillCoolTimePercent(EAbilityType inType);
	
	//=================================================================================
public:
	bool GetCombatState();
	// !------------------------------------------------------------

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }

	// !MyCode------------------------------------------------------
public:
	int Exp;
	int Level;
	int TempLevel;

	void LevelUpEvent();

	UFUNCTION(BlueprintPure)
	int GetCharacterLevel();

	UFUNCTION(BlueprintPure)
	float GetCharacterExpPercent();

	TMap<EAbilityType, bool> SkillActivating;

	bool GetOtherSkillActivating(EAbilityType inType);
	
	// * 임시 죽는애니메이션 play
	UFUNCTION(BlueprintPure)
	bool IsDead() const;
	
	// * 디버그on/off
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool DebugOnOff = 0;

	// * 줌인아웃 & 카메라조종가능
	bool IsZoomed = false;
	bool CanZoom = true;
	float ZoomInterpTime = 6;

	UPROPERTY()
	float MyTargetArmLength = 400.0f;

	UPROPERTY()
	FVector MyTargetArmLocation = FVector(0, 0, 45);

	UPROPERTY()
	FVector MyCameraLocation = FVector(0, 0, 70);

	bool CanCameraControl = true;

	// * Pawn Rotate
	bool IsRotate = false;
	bool CanRotate = false;

	// * 전투state
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool InCombat = false;

	FTimerHandle CombatStateTHandle;

	// * 초기속력(default), 현재속력(this) 로 구분.
	// TODO ThisSpeed = DefaultSpeed + DefaultSpeed * 아이템효과
	UPROPERTY(EditAnywhere, Category = "BaseMovement")
	float DefaultSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "BaseMovement")
	float ThisSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "BaseMovement")
	float DefaultJogSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "BaseMovement")
	float ThisJogSpeed = 1200.f;

	// * w눌린상태>달리기가능   달리는상태>w때기>달리기종료
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsJog = false;

	bool CanJog = true;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsWPressed = false;

	bool IsSPressed = false;

	// * 연속점프횟수 bp에서 간접적으로 건드리기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseMovement", meta = (AllowPrivateAccess = "true"))
	int32 ThisJumpMaxCount = 1;

	// * 체력 및 방어력 및 특수
	// TODO 방어력 기능 구현
	float TempMaxHealth;
	
	UPROPERTY(EditDefaultsOnly, Category = "Defensive")
	int MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, Category = "Defensive")
	int Health;

	UPROPERTY(EditDefaultsOnly, Category = "Defensive")
	int Armor = 0;

	UPROPERTY(EditAnywhere, Category = "Defensive")
	bool DamageImmunity = false; // true > 무적
	// TODO 각종 공격효과 무효 bool

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool TakeDamageAnim = false;

	bool CanTakeDamageAnim = true;
	FTimerHandle TakeDamageHandle;

	// * 공격관련 변수
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "CombatProp")
	float FireRate = 0.25f;
	// !------------------------------------------------------------

private:
	// 레일 타는 거 임시구현!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	virtual void Landed(const FHitResult& Hit) override;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MoveForward = 0;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MoveRight = 0;

public:
	DECLARE_DYNAMIC_DELEGATE(FDelegateInteractTask);

	//상호작용 액터용 댈리게이트, 컨트롤러에서 입력 시 Excute 
	FDelegateInteractTask DelegateInteractTask;
};
