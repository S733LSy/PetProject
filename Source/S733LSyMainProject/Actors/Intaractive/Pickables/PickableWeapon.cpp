// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableWeapon.h"
#include "S733LSyMainProjectTypes.h"
#include "Characters/SMBaseCharacter.h"
#include "Inventory/Items/InventoryItem.h"
#include "Inventory/Items/Equipables/WeaponInventoryItem.h"
#include "Utils/SMDataTableUtils.h"

APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void APickableWeapon::Interact(ASMBaseCharacter* Character)
{
	FWeaponTableRow* WeaponRow = SMDataTableUtils::FindWeaponData(DataTableID);
	if (WeaponRow)
	{
		TWeakObjectPtr<UWeaponInventoryItem> Weapon = NewObject<UWeaponInventoryItem>(Character); // создаём новый объект
		Weapon->Initialize(DataTableID, WeaponRow->WeaponItemDescrition); // Инициализируем еге
		Weapon->SetEquipWeaponClass(WeaponRow->EquipableActor); // выставляем класс
		Character->PickupItem(Weapon, 1, Type); // метод поднятия айтема
		Destroy();
	}
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}
