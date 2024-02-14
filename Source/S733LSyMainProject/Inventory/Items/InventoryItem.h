// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S733LSyMainProjectTypes.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class UInventoryItem;
class ASMBaseCharacter;
class APickableItem;
class AEquipableItem;

USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FText Name; // имя
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	UTexture2D* Icon; // иконка
};

USTRUCT(BlueprintType) // будем находить соответствие между поднимаемым эктором и эктором экипировки
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<AEquipableItem> EquipableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	FInventoryItemDescription WeaponItemDescrition;
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<UInventoryItem> InventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FInventoryItemDescription InventoryItemDescription;
};

USTRUCT(BlueprintType)
struct FAmmoTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<APickableItem> PickableActorClass; //класс подбираемого объекта

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	EAmunitionType AmunitionType; //тип амуниции
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FInventoryItemDescription InventoryItemDescription;
	
};


UCLASS(Blueprintable) 
class S733LSYMAINPROJECT_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In);
	
	FName GetDataTableId() const;
	const FInventoryItemDescription& GetDescription() const;

	virtual bool IsEquipable() const;
	virtual bool IsConsumable() const;

	virtual bool Consume(ASMBaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Consume, return false;);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	FName DataTableID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	FInventoryItemDescription Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	bool bIsEquipable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	bool bIsConsumable = false; // потребляемый ли айтем или нет

private:
	bool bIsInitialized = false;
};
