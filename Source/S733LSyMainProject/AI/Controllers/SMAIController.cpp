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
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors); // Экторы, которых мы сейчас видим (класс чувств, Массив в который будут возвращены данные значения)

	// Нахождение ближайшего эктора
	AActor* ClosestActor = nullptr; // ссылка на ближайшего эктора
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
