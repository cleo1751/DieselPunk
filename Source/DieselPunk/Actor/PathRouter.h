// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*
*	박스로 구성한 영역안의 랜덤 위치에서 몬스터를 소환하는 액터입니다.
*	
*/

#include "GameFramework/Actor.h"
#include "../Interface/DpManagementTargetInterFace.h"
#include "PathRouter.generated.h"

class USplineComponent;
class UBoxComponent;

UCLASS()
class DIESELPUNK_API APathRouter : public AActor, public IDpManagementTargetInterFace
{
	GENERATED_BODY()

	UPROPERTY()
	USceneComponent* SceneRoot = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent *Mesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "MYDP_Setting")
	bool DebugOnOff = false;	

	/////////////////////////////////////////////////////////////////////
	// for info Management //

	UPROPERTY(EditInstanceOnly, Category = "MYDP_Setting")
	TObjectPtr<APathRouter> NextPathRouter;

	// n번째 PathRouter 노드
	UPROPERTY(VisibleInstanceOnly, Category = "MYDP_Setting")
	int32 PathRouterNum = 0;				
	
	/////////////////////////////////////////////////////////////////////
	// for Box , Poligon , MakeRandLoc //

	// 영역을 만들 박스 컴포넌트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;		
	
	TArray<FVector> RectanglePoints;			// 박스의 꼭짓점을 담을 배열
	
	
public:	
	// Sets default values for this actor's properties
	APathRouter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called change info in editor
	virtual void OnConstruction(const FTransform& InTransform)override;

private:
	// 스플라인 포인트를 기반으로 직사각형의 점을 PolygonPoints에 담습니다.
	void MakeRectangleBySplinePoints();

public:
	// 연결된 PathRouter를 모두 등록합니다.
	void RegistPathRouter(TMap<int32, TObjectPtr<APathRouter>>& inPathRouterNodes, int32& inPathRouterNodeNum);

	// 투영값을 통해 위치를 반환합니다.
	FVector MakeGoalLocByProportion(FVector2D inProportion);

	int32 GetPathRouterNumber() { return PathRouterNum; }
};
