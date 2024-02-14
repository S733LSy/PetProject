// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Intaractive/Pickables/PickableAmmo.h"
#include "Inventory/Items/InventoryItem.h"
#include "Inventory/Items/Ammunition/InventoryAmmoItem.h"

namespace SMDataTableUtils
{
	FWeaponTableRow* FindWeaponData(const FName WeaponID);
	FItemTableRow* FindInventoryItemData(const FName ItemID);
	FAmmoTableRow* FindInventoryAmmoData(const FName AmmoID);
};
