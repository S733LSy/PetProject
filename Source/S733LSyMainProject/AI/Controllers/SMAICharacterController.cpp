// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAICharacterController.h"
#include "AI/Characters/SMAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"
#include "S733LSyMainProjectTypes.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"

void ASMAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ASMAICharacter>(), TEXT("ASMAICharacterController::SetPawn AICharacterController can possess only SMAICharacter"));
		CachedAICharacter = Cast<ASMAICharacter>(InPawn);
		RunBehaviorTree(CachedAICharacter->GetBehaviorTree());
		SetupPatrolling();
	}
	else
	{
		CachedAICharacter = nullptr;
	}
}

void ASMAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

	if (!CachedAICharacter.IsValid())
	{
		return;
	}

	TryMoveToNextTarget();
}

void ASMAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!Result.IsSuccess())
	{
		return;
	}
	TryMoveToNextTarget();
}

void ASMAICharacterController::SetupPatrolling()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	if (PatrollingComponent->CanPatrol())
	{
		FVector ClosestWayPoint = PatrollingComponent->SelectClosestWayPoint();
		if (IsValid(Blackboard))
		{
			Blackboard->SetValueAsVector(BB_NextLocation, ClosestWayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr); // передаём в CurrentTarget Closestactor
		}
		bIsPatrolling = true;
	}
}

void ASMAICharacterController::TryMoveToNextTarget()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosestActor))
	{
		if (IsValid(Blackboard))
		{
			Blackboard->SetValueAsObject(BB_CurrentTarget, ClosestActor); // передаём в CurrentTarget Closestactor
			SetFocus(ClosestActor, EAIFocusPriority::Gameplay);
		}
		bIsPatrolling = false;
	}
	else if (PatrollingComponent->CanPatrol())
	{
		FVector WayPoint = bIsPatrolling ? PatrollingComponent->SelectNextWayPoint() : PatrollingComponent->SelectClosestWayPoint();
		if (IsValid(Blackboard))
		{
			ClearFocus(EAIFocusPriority::Gameplay);
			Blackboard->SetValueAsVector(BB_NextLocation, WayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr); // передаём в CurrentTarget Closestactor
		}
		bIsPatrolling = true;
	}
}

bool ASMAICharacterController::IsTargetReached(FVector TargetLocation) const
{
	return (TargetLocation - CachedAICharacter->GetActorLocation()).SizeSquared() <= FMath::Square(TargetReachRadius);
}
