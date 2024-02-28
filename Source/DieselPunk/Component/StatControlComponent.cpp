// Fill out your copyright notice in the Description page of Project Settings.


#include "StatControlComponent.h"
#include "../Character/CharacterBase.h"
#include "../Raw/Buff.h"

#include <GameFramework/CharacterMovementComponent.h>

#include "DieselPunk/Data/StatDataTable.h"
#include "DieselPunk/Manager/DatatableManager.h"

// Sets default values for this component's properties
UStatControlComponent::UStatControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatControlComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	if(!Owner.IsValid())
		return;
	
	//스탯 초기화
	Stat.Empty();
	for(ECharacterStatType statType : TEnumRange<ECharacterStatType>())
	{
		if(statType == ECharacterStatType::Count)
			continue;

		Stat.FindOrAdd(statType) = 0.0;
	}

	DelegateChangeStat.Clear();
	DelegateChangeStat.AddUObject(this, &UStatControlComponent::SetStatDelegateFunction);

	if(ActorName == TEXT(""))
	{
		UE_LOG(LogLoad, Error, TEXT("%s의 ActorName이 등록되지 않았습니다. 데이터 테이블에서 스탯을 불러올 수 없습니다."), *Owner->GetName())
		return;
	}

	const FStatDataTable* data = FDataTableManager::GetInstance()->GetData<FStatDataTable>(EDataTableType::Stat, ActorName);

	if(!data)
	{
		UE_LOG(LogLoad, Error, TEXT("ActorName: %s에 해당하는 데이터 열이 없습니다. 데이터 테이블에서 스탯을 불러올 수 없습니다."), *Owner->GetName())
		return;
	}

	SetStat(ECharacterStatType::Level, 1);
	SetStat(ECharacterStatType::Exp, 0);

	SetStat(ECharacterStatType::MaxHp,			 data->Hp);
	SetStat(ECharacterStatType::Hp,				 data->Hp);
	SetStat(ECharacterStatType::HpRecoverySpeed, data->HpRecoverySpeed);
	SetStat(ECharacterStatType::Atk,			 data->Atk);
	SetStat(ECharacterStatType::AtkSpeed,		 data->AtkSpeed);
	SetStat(ECharacterStatType::Def,			 data->Def);
	SetStat(ECharacterStatType::ArmorPen,		 data->ArmorPen);
	SetStat(ECharacterStatType::CriticalPer,	 data->CriticalPer);
	SetStat(ECharacterStatType::CriticalAtk,	 data->CriticalAtk);
	SetStat(ECharacterStatType::MoveSpeed,		 data->MoveSpeed);
	SetStat(ECharacterStatType::JumpCount,		 data->JumpCount);
	SetStat(ECharacterStatType::Luck,			 data->Luck);
	SetStat(ECharacterStatType::CoolDown,		 data->CoolDown);
	SetStat(ECharacterStatType::AttackMaxRange,	 data->AttackMaxRange);
	SetStat(ECharacterStatType::AttackMinRange,	 data->AttacMinRange);
	SetStat(ECharacterStatType::Cost,			  data->Cost);
}

void UStatControlComponent::SetStatDelegateFunction(TWeakObjectPtr<AActor> InActor, ECharacterStatType InStatType, int32 InValue)
{
	if(!InActor.IsValid())
		return;

	if(InStatType == ECharacterStatType::MoveSpeed)
	{
		ACharacterBase* charBase = Cast<ACharacterBase>(InActor);
		UActorComponent* comp = charBase->GetComponentByClass(UCharacterMovementComponent::StaticClass());
		if(comp == nullptr)
			return;

		UCharacterMovementComponent* movementComponent = Cast<UCharacterMovementComponent>(comp);
		movementComponent->MaxWalkSpeed = InValue;
		movementComponent->MaxWalkSpeedCrouched = InValue / 2;
	}
}

void UStatControlComponent::SetStat(ECharacterStatType InStatType, int32 InValue)
{
	int32* stat = Stat.Find(InStatType);
	if(!stat)
		return;

	*stat = *stat + InValue;

	if(DelegateChangeStat.IsBound())
		DelegateChangeStat.Broadcast(Owner, InStatType, *stat);
}


// Called every frame
void UStatControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	for(TMap<FString, FBuff*>::TIterator iter = Buffs.CreateIterator(); iter; ++iter)
	{
		if(iter->Value->Tick(DeltaTime))
			iter.RemoveCurrent();
	}
}

const int32& UStatControlComponent::GetStat(ECharacterStatType InStatType)
{
	const int32* stat = Stat.Find(InStatType);
	if(!stat)
		return INVALID_STAT;

	return *stat;
}

void UStatControlComponent::AddBuff(const FString& InBuffName)
{
	FBuff** findTemp = Buffs.Find(InBuffName);
	
	FBuff* buff = nullptr;
	if(findTemp != nullptr)
	{
		buff = *findTemp;
		buff->Initialize();
	}
	else
	{
		buff = new FBuff(this, InBuffName);
		Buffs.Add(InBuffName, buff);
	}
}