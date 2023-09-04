// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterPC.h"
#include "CharacterNPC.generated.h"

/**
 *
 */


UCLASS()
class DIESELPUNK_API ACharacterNPC : public ACharacterPC
{
	GENERATED_BODY()

public:
	ACharacterNPC();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyCustomCategory", meta = (AllowPrivateAccess = "true"))
	UPlayerSkill *MeleeAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyCustomCategory", meta = (AllowPrivateAccess = "true"))
	UPlayerSkill *ProjectileAttack;

public:

	UPROPERTY(EditAnywhere, Category = "MyCustomCategory")
	UParticleSystem *GranadeMuzzleEffect;

	// 상태 UI 위젯을 생성한다.
	virtual void CreateStatusUI() override;
	
	float DoMeleeAttack();
	void TempDoMeleeAttack();
	void DoProjectileAttack();
};
