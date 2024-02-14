// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S733LSyMainProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "InventoryAmmoItem.generated.h"

UCLASS()
class S733LSYMAINPROJECT_API UInventoryAmmoItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	UInventoryAmmoItem();

	void SetAmmoType(EAmunitionType AmmoType_In);

protected:
	UPROPERTY()
	EAmunitionType AmunitionType;
};
