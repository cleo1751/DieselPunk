// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterPC.h"
#include "NavigationData.h"
#include "DieselPunk/AI/BTTask_ProjectileAttack.h"
#include "CharacterNPC.generated.h"

class UHousingActorComponent;
class USkillBase;
class UEnemyStatusUI;

struct FProportion
{
	float Width = 0;
	float Length = 0;
};

UCLASS()
class DIESELPUNK_API ACharacterNPC : public ACharacterBase
{
	GENERATED_BODY()
	
protected:
	/////////////////////////////////////////////////////////////////////
	// for UI //
	
	UPROPERTY()
	TWeakObjectPtr< UEnemyStatusUI > EnemyStatusUI;		// 상태 UI 포인터

	FTimerHandle EnemyStatusUITHandle;

	
public:
	/////////////////////////////////////////////////////////////////////
	// for Character info Management //
	
	UPROPERTY(EditAnywhere, Category = "MYDP_info")
	ENPCType NPCType = ENPCType::Enemy;						// NPC 타입

	
	/////////////////////////////////////////////////////////////////////
	// for skill //
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MYDP_Skill", meta = (AllowPrivateAccess = "true"))
	USkillBase *MeleeAttack;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MYDP_Skill", meta = (AllowPrivateAccess = "true"))
	USkillBase *ProjectileAttack;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MYDP_Skill", meta = (AllowPrivateAccess = "true"))
	USkillBase *TargetAttack;
	
	
	//포탑의 사이즈입니다. 1그리드 = 100입니다.
	UPROPERTY(EditDefaultsOnly, Category="MYDP_Setting", meta=(AllowPrivateAccess="true"))
	int32 GridSize = 1;

	
	TArray<FVector> GoalLocArray;	//도달할 목표위치 배열
	int32 GoalLocNum = 0;			//도달할 목표위치 순서

	TArray<FVector> ShortestPath;	//최단거리
	FVector GoalLoc = FVector::ZeroVector;			//도달할 타겟 위치
	TWeakObjectPtr<AActor> Target = nullptr;		//공격할 타겟
	FVector BlockedTargetLoc = FVector::ZeroVector;	//공격할 타겟 위치

protected:
	ACharacterNPC();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	// 상태 UI 위젯을 생성한다.
	virtual void CreateStatusUI() override;
	
	// UI 컨트롤 하기 위한 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;

	// UI 타이머 컨트롤
	void HandleStatusUI();

public:
	// 몬스터 스킬
	void DoMeleeAttack();
	void DoProjectileAttack();
	void DoTargetAttack();
	
	// 스폰시 '몬스터'의 TargetArray를 설정합니다.
	void SetTargetArray(TArray<FVector> inTargetArray);
	// '몬스터'의 Target을 설정합니다.
	void SetEnemyTarget();
	FVector GetTargetLoc() { return GoalLoc; }
	TWeakObjectPtr<AActor> GetAttackTarget() { return Target; }

	
	// 길이 막혔을 때, 파괴시 진행할 수 있는 포탑의 위치를 찾습니다.
	bool FindShortestPath(const FVector& InEndLocation);
	const TArray<FVector>& GetShortestPath() { return ShortestPath; } 
	int32 GetGridSize() const { return GridSize; }

	// 길이 막혔을 때, '몬스터'의 타겟을 지정합니다.
	bool SetBlockedAttackTarget(TWeakObjectPtr<AActor> InTarget, const TArray<FVector>& InPath = TArray<FVector>(), int InIndex = -1);
	FVector GetBlockedAttackTargetLoc() { return BlockedTargetLoc; }
	

	/////////////////////////////////////////////////////////////////////
	// Navigation
public:
	// =========================================
	// NavigationTestingActor를 참고한 코드입니다.
	UPROPERTY(transient)
	TObjectPtr<ANavigationData> MyNavData;

	// inStartLoc to inEndLoc 경로탐색 // 기존의 경로탐색 로직을 그대로 따라합니다.(아마도)
	FNavPathSharedPtr SearchPathTo(const FVector inStartLoc, const FVector inEndLoc);
	
	// FPathFindingQuery Set
	FPathFindingQuery BuildPathFindingQuery(const FVector inStartLoc, const FVector inEndLoc) const;
	// =========================================

	TArray<FVector> MyPathPoints;	// 몬스터의 전체경로를 담을 배열
	
	// 전체 경로를 탐색합니다. // 몬스터 스폰시, 포탑 설치/파괴시, Target이 Nexus로 업데이트될 때 호출합니다.
	void UpdatePath();
};
