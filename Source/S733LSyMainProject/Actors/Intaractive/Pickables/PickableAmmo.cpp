// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableAmmo.h"
#include "Characters/SMBaseCharacter.h"
#include "Utils/SMDataTableUtils.h"

APickableAmmo::APickableAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);
}

void APickableAmmo::Interact(ASMBaseCharacter* Character)
{
	FAmmoTableRow* AmmoRow = SMDataTableUtils::FindInventoryAmmoData(DataTableID);
	if (AmmoRow)
	{
		TWeakObjectPtr<UInventoryAmmoItem> Ammo = NewObject<UInventoryAmmoItem>(Character);
		Ammo->Initialize(DataTableID, AmmoRow->InventoryItemDescription);
		Ammo->SetAmmoType(AmmoRow->AmunitionType);
		if (GetCount() == 0)
		{
			Character->PickupItem(Ammo, PickableAmmoCount, Type);
		}
		else
		{
			Character->PickupItem(Ammo, GetCount(), Type);	
		}
		Destroy();
	}
}

FName APickableAmmo::GetActionEventName() const
{
	return ActionInteract;
}
