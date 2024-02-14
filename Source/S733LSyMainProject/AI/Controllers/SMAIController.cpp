// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"

ASMAIController::ASMAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* ASMAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
	if (!IsValid(GetPawn()))
	{
		return nullptr;
	}

	TArray<AActor*> SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors); // ������, ������� �� ������ ����� (����� ������, ������ � ������� ����� ���������� ������ ��������)

	// ���������� ���������� ������
	AActor* ClosestActor = nullptr; // ������ �� ���������� ������
	float MinSquaredDistance = FLT_MAX;
	FVector PawnLocation = GetPawn()->GetActorLocation();

	for (AActor* SennActor : SensedActors)
	{
		float CurrentSquaredDistance = (PawnLocation - SennActor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SennActor;
		}
	}
	return ClosestActor;
}
