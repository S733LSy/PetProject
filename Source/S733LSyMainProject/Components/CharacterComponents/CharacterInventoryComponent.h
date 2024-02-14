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
	DECLARE_DELEGATE(FInventorySlotUpdate); // ������� ��� ������� �����

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItem> Item; // ��������� �� ��� �����, ������� �������� ������ ���������

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ������� ��������� ����� ���� � ������ ���������� ��� ���������� ���������
	int32 Count = 0;

	void BindOnInventorySlotUpdate(const FInventorySlotUpdate& CallBack) const; // ����� �������� �� �������
	void UnBindOnInventorySlotUpdate(); // ����� ������� �� ��������
	void UpdateSlotState(); // ����� ���������� ��������� �����
	void ClearSlot(); // ������� �����
	
private:
	mutable FInventorySlotUpdate OnInventorySlotUpdate;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void OpenViewInventory(APlayerController* PlayerController); // ����� �������� ��������� (�������)
	void CloseViewInventory(); // ����� �������� ��������� (������)
	bool IsViewVisible() const; // ����� ��� �� ����� ������ ���������

	int32 GetCapacity() const; // ��������� ���������� ������
	bool HasFreeSlot() const; // �������� ���� �� ��������� ����
	void AddItemToFreeSlot(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, bool& Result);

	bool AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, EPickableItemType ItemType); // ����� ��� ���������� ��������� � ���������,
	//�������� ������ ������ �� ����� � ���������� ������� ���������

	bool RemoveItem(FName ItemID); // �����, ������� �������� ������� ����������� �����

	TArray<FInventorySlot> GetAllItemsCopy() const; // ����������� ���� ������� ��������� ���������
	TArray<FText> GetAllItemsNames() const; // ���������� ������ ��� ���� ���������, ������� ���� � ���������

protected:
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FInventorySlot> InventorySlots; // ������ ������ ���������

	UPROPERTY(EditAnywhere, Category = "View settings")
	TSubclassOf<UInventoryViewWidget> InventoryViewWidgetClass; // ������ �� ������ ��������� � ��� ��������

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory setting", meta = (ClampMin = 1, UIMin = 1))
	int32 Capacity = 16; // ���������� ������
	
	virtual void BeginPlay() override;

	void CreateViewWidget(APlayerController* PlayerController); // ����� ��� �������� �������

	FInventorySlot* FindItemSlot(FName ItemID); // ����� ����� � ���������� ��������� ���������

	FInventorySlot* FindFreeSlot(); // ����� ���������� �����

private:
	UPROPERTY() // ��������
	UInventoryViewWidget* InventoryViewWidget; // ������ �� ������ ���������

	int32 ItemsInInventory; // ���������� ��������� � ���������
};
