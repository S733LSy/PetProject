// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrollingComponent.generated.h"

class APatrollingPath;

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	Circle,
	PingPong
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool CanPatrol() const;

	FVector SelectClosestWayPoint();
	FVector SelectNextWayPoint();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Type")
	EPatrolType PatrolType = EPatrolType::Circle;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	APatrollingPath* PatrollingPath;

	void SwitchPatrolPath();

private:
	int32 CurrentWayPointIndex = -1;

	bool bIsReverseDirection = false;
};
