// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryAmmoItem.h"

UInventoryAmmoItem::UInventoryAmmoItem()
{
	bIsConsumable = false;
	bIsEquipable = false;
}

void UInventoryAmmoItem::SetAmmoType(EAmunitionType AmmoType_In)
{
	AmunitionType = AmmoType_In;
}