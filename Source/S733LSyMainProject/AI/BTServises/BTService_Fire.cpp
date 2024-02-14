// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Fire.h"
#include "AIController.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	ARangeWeaponItem* RangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (!IsValid(RangeWeapon))
	{
		return; // не выполняем сервис
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget))
	{
		Character->StopFire();
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistSq > FMath::Square(MaxFireDistance))
	{
		Character->StopFire();
		return;
	}

	if (!(RangeWeapon->IsReloading() || RangeWeapon->IsFiring()))
	{
		Character->StartFire();
	}
}
