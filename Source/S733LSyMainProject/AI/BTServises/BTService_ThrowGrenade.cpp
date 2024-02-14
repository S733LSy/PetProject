// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_ThrowGrenade.h"

#include "AIController.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

UBTService_ThrowGrenade::UBTService_ThrowGrenade()
{
	NodeName = "Throw";
}

void UBTService_ThrowGrenade::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard))
	{
		return;
	}

	ASMBaseCharacter* Character = Cast<ASMBaseCharacter>(AIController->GetPawn()); // получение персонажа
	if (!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* CharacterEquipmentComponent = Character->GetCharacterEquipmentComponent();
	int32 CurrentThrowableAmmo = CharacterEquipmentComponent->GetCurrentThrowableAmmo();
	if (CurrentThrowableAmmo == 0)
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget))
	{
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistSq > FMath::Square(MaxThrowDistance))
	{
		return;
	}

	Character->EquipPrimaryItem();

}
