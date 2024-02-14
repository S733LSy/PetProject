
#include "SMDataTableUtils.h"

#include "S733LSyMainProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "Engine/DataTable.h"

FWeaponTableRow* SMDataTableUtils::FindWeaponData(const FName WeaponID)
{
	static const FString contextString(TEXT("Find Weapon Data"));
	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/S733LSyMainProject/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, contextString);
	
}

FItemTableRow* SMDataTableUtils::FindInventoryItemData(const FName ItemID)
{
	static const FString contextString(TEXT("Find Item Data"));
	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/S733LSyMainProject/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if (InventoryItemDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, contextString);
}

FAmmoTableRow* SMDataTableUtils::FindInventoryAmmoData(const FName AmmoID)
{
	static const FString contextString(TEXT("Find Ammo Data"));
	UDataTable* InventoryAmmoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/S733LSyMainProject/Core/Data/DataTables/DT_InventoryAmmoList.DT_InventoryAmmoList"));

	if (InventoryAmmoDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryAmmoDataTable->FindRow<FAmmoTableRow>(AmmoID, contextString);
}
