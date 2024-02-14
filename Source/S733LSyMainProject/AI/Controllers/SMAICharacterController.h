// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/SMAIController.h"
#include "SMAICharacterController.generated.h"

class ASMAICharacter;
UCLASS()
class S733LSYMAINPROJECT_API ASMAICharacterController : public ASMAIController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	void SetupPatrolling();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float TargetReachRadius = 100.0f;

private:
	void TryMoveToNextTarget();

	TWeakObjectPtr<ASMAICharacter> CachedAICharacter;

	bool bIsPatrolling = false;
	
	bool IsTargetReached(FVector TargetLocation) const;
};
