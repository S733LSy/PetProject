// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EPickableItemType : uint8
{
	Powerup,
	Weapon,
	Ammo
};

class UInventoryViewWidget;
class UInventoryItem;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FInventorySlotUpdate); // делегат для апдейта слота

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItem> Item; // указатель на тот айтем, который содержит данный контейнер

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // сколько элементов может быть в данном контейнере для однотипных элементов
	int32 Count = 0;

	void BindOnInventorySlotUpdate(const FInventorySlotUpdate& CallBack) const; // метод подписки на делегат
	void UnBindOnInventorySlotUpdate(); // метод отписки от делегата
	void UpdateSlotState(); // метод обновления состояния слота
	void ClearSlot(); // очистка слота
	
private:
	mutable FInventorySlotUpdate OnInventorySlotUpdate;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void OpenViewInventory(APlayerController* PlayerController); // метод открытия инвенторя (виджета)
	void CloseViewInventory(); // метод закрытия инвенторя (виджет)
	bool IsViewVisible() const; // виден или не виден виджет инвенторя

	int32 GetCapacity() const; // получения количества слотов
	bool HasFreeSlot() const; // проверка есть ли свободный слот
	void AddItemToFreeSlot(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, bool& Result);

	bool AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, EPickableItemType ItemType); // метод для добавления элементов в инвентарь,
	//получает слабую ссылку на айтем и количество сколько добавляем

	bool RemoveItem(FName ItemID); // метод, который позволит удалить определённый айтем

	TArray<FInventorySlot> GetAllItemsCopy() const; // копирование всех текущих элементов инвенторя
	TArray<FText> GetAllItemsNames() const; // возвращает массив имён всех элементов, которые есть в инвенторе

protected:
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FInventorySlot> InventorySlots; // массив слотов инвенторя

	UPROPERTY(EditAnywhere, Category = "View settings")
	TSubclassOf<UInventoryViewWidget> InventoryViewWidgetClass; // ссылка на виджет инвенторя и его потомков

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory setting", meta = (ClampMin = 1, UIMin = 1))
	int32 Capacity = 16; // количество слотов
	
	virtual void BeginPlay() override;

	void CreateViewWidget(APlayerController* PlayerController); // метод для создания виджета

	FInventorySlot* FindItemSlot(FName ItemID); // посик слота с подходящим элементом инвенторя

	FInventorySlot* FindFreeSlot(); // поиск свободного слота

private:
	UPROPERTY() // свойство
	UInventoryViewWidget* InventoryViewWidget; // ссылка на виджет инвенторя

	int32 ItemsInInventory; // количество предметов в инвентаре
};
