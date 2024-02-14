
#include "AIPatrollingComponent.h"
#include "Actors/Navigation/PatrollingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWayPoints().Num() > 0;
}

FVector UAIPatrollingComponent::SelectClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrollingPath->GetWayPoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSqDistance = FLT_MAX;
	for (int32 i = 0; i < WayPoints.Num(); ++i)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if (CurrentSqDistance < MinSqDistance)
		{
			MinSqDistance = CurrentSqDistance;
			ClosestWayPoint = WayPointWorld;
			CurrentWayPointIndex = i;
		}
	}
	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWayPoint()
{
	TArray<FVector> WayPoints = PatrollingPath->GetWayPoints();

	SwitchPatrolPath();

	FTransform PathTransform = PatrollingPath->GetActorTransform();
	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
	return WayPoint;
}

void UAIPatrollingComponent::SwitchPatrolPath()
{
	if (PatrolType == EPatrolType::Circle)
	{
		++CurrentWayPointIndex;
		if (CurrentWayPointIndex == PatrollingPath->GetWayPoints().Num())
		{
			CurrentWayPointIndex = 0;
		}
	}

	if (PatrolType == EPatrolType::PingPong)
	{
		++CurrentWayPointIndex;
		if (!bIsReverseDirection && CurrentWayPointIndex == 1)
		{
			++CurrentWayPointIndex;
		}
		else if (CurrentWayPointIndex == PatrollingPath->GetWayPoints().Num() && bIsReverseDirection == false)
		{
			bIsReverseDirection = true;
			CurrentWayPointIndex = CurrentWayPointIndex - 2;
		}
		else if (bIsReverseDirection && CurrentWayPointIndex > 1)
		{
			CurrentWayPointIndex = CurrentWayPointIndex - 2;
		}
		else if (bIsReverseDirection && CurrentWayPointIndex == 1)
		{
			bIsReverseDirection = false;
		}
	}
}
