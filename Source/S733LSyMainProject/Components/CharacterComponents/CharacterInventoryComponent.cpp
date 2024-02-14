
#include "CharacterInventoryComponent.h"

#include "Inventory/Items/InventoryItem.h"
#include "UI/Widget/Inventory/InventoryViewWidget.h"

void FInventorySlot::BindOnInventorySlotUpdate(const FInventorySlotUpdate& CallBack) const
{
	OnInventorySlotUpdate = CallBack;
}

void FInventorySlot::UnBindOnInventorySlotUpdate()
{
	OnInventorySlotUpdate.Unbind();
}

void FInventorySlot::UpdateSlotState()
{
	OnInventorySlotUpdate.ExecuteIfBound();
}

void FInventorySlot::ClearSlot()
{
	Count = 0;
	Item = nullptr;
	UpdateSlotState();
}

void UCharacterInventoryComponent::OpenViewInventory(APlayerController* PlayerController) // ����� �������� ���������
{
	if (!IsValid(InventoryViewWidget)) // �������� �� ���������� ������� ���������
	{
		CreateViewWidget(PlayerController); // ���� �� �������, �� ������ ���
	}

	if (!InventoryViewWidget->IsVisible()) // �������� �� ��������� �������
	{
		InventoryViewWidget->AddToViewport(); // ���� �� �����, �� ��������� �� �������
	}
}

void UCharacterInventoryComponent::CloseViewInventory() // ����� �������� ���������
{
	if (InventoryViewWidget->IsVisible()) // �������� ������� �� ���������
	{
		InventoryViewWidget->RemoveFromParent(); // ���� �� �����, �� ������� ���
	}
}

bool UCharacterInventoryComponent::IsViewVisible() const // ������ ����� ������ ��� ���
{
	bool Result = false; // ������ ����������
	if (IsValid(InventoryViewWidget)) // ���� ������ �������
	{
		Result = InventoryViewWidget->IsVisible(); // ��������� ����� ����� � ���������� �� ����, ����� ������ ��� ���
	}
	return Result; // ���� �� �������� ������ ���������, �� ������������ ������ ��������, ��� ����� � ���� ������� ���������� Result
}

int32 UCharacterInventoryComponent::GetCapacity() const // ������ ���������� ������
{
	return Capacity; // ���������� ���������� ������
}

bool UCharacterInventoryComponent::HasFreeSlot() const
{
	return ItemsInInventory < Capacity; // ���������� true, ���� ���������� ��������� � ��������� ������ ��� ������, � ���������� ������� false
}

void UCharacterInventoryComponent::AddItemToFreeSlot(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, bool& Result)
{
	FInventorySlot* FreeSlot = FindFreeSlot(); // ���� ��������� ����
	if (FreeSlot != nullptr) // ���� ��������� ���� �� ����� ������� ������
	{ // ����� �������� �� ��������� ������, ���� �� ����
		FreeSlot->Item = ItemToAdd; // � �����, ������� �������� ������� �� ��������� ����������� ����� �� ����������
		FreeSlot->Count = Count; // ����� ������� �������� � ���������� � ������� ������
		ItemsInInventory++; // � ���������� ��������� � ��������� ��������� 1, ��� ���������� ������� ������
		Result = true; // ��������� ���������� ������ ���
		FreeSlot->UpdateSlotState(); // � ���������� ����� �������� ����� �������
	}
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, EPickableItemType ItemType)
{
	if (!ItemToAdd.IsValid() || Count < 0) // ���� ����� �� ���������� �� ������� ��� ��� ���������� ������ ����
	{
		return false; // �� ���������� ����������� �� �����
	}

	bool Result = false; // ������ ���������� - ��������� ����������
	int32 MaxSlotCount = 0;
	
	switch (ItemType)
	{
	case (uint8)EPickableItemType::Ammo:
		{
			MaxSlotCount = 300;
			break;
		}
	case (uint8)EPickableItemType::Weapon:
		{
			MaxSlotCount = 1;
			break;
		}
	case (uint8)EPickableItemType::Powerup:
		{
			MaxSlotCount = 5;
			break;
		}
	}

	for (FInventorySlot& Slot : InventorySlots) // �������� �� ����� ������� ������
	{
		if (Slot.Item.IsValid()) // ���� ���� �������
		{
			if (Slot.Item->GetDescription().Name.ToString() == ItemToAdd->GetDescription().Name.ToString())
			{
				if (Slot.Count == MaxSlotCount)
				{
					continue;
				}
				if ((Slot.Count + Count) <= MaxSlotCount)
				{
					Slot.Item = ItemToAdd;
					int32 OldCount = Slot.Count;
					Slot.Count = OldCount + Count;
					Result = true;
					Slot.UpdateSlotState();
					return Result;
				}
				else
				{
					Slot.Item = ItemToAdd;
					int32 OldCount = Slot.Count;
					Slot.Count = Slot.Count + (MaxSlotCount - OldCount);
					int32 CountToAdd = Count - (MaxSlotCount - OldCount);
					AddItemToFreeSlot(ItemToAdd, CountToAdd, Result);
					Slot.UpdateSlotState();
					return Result;
				}
			}
		}
	}
	
	AddItemToFreeSlot(ItemToAdd, Count, Result);

	return Result;
}

bool UCharacterInventoryComponent::RemoveItem(FName ItemID)
{
	FInventorySlot* ItemSlot = FindItemSlot(ItemID); // ���� ���� � ����, ������� ��������� � �����
	if (ItemSlot != nullptr) // ���� ����������
	{
		InventorySlots.RemoveAll([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableId() == ItemID; }); // ������ ������ �� �����, ������� ����� ItemID
		return true; // ��������� ���, ���� �������
	}
	return false; // false, ���� �� �������
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FText> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FText> Result; // ������ � �����������
	for (const FInventorySlot& Slot : InventorySlots) // �������� �� ����� ������� ������
	{
		if (Slot.Item.IsValid()) // ���� ���� �������
		{
			Result.Add(Slot.Item->GetDescription().Name); // �� � ������ ���������� ��������� ��� ����� ������
		}
	}
	return Result;
}

void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InventorySlots.AddDefaulted(Capacity); // ��������� ��������� ������� ������� �� ���������� ��������� ������
}

void UCharacterInventoryComponent::CreateViewWidget(APlayerController* PlayerController)
{
	if (IsValid(InventoryViewWidget)) // ���� ������� ������, �� �������, ����� ��� ������ ���������
	{
		return;
	}
	
	if (!IsValid(PlayerController) || !IsValid(InventoryViewWidgetClass)) // ���� �� ������� ���������� ������ ��� �� ������� ����� �������
	{
		return; // �� ������� �� ������
	}

	InventoryViewWidget = CreateWidget<UInventoryViewWidget>(PlayerController, InventoryViewWidgetClass); // ������ ������
	InventoryViewWidget->InitializeViewWidget(InventorySlots); // �������� ����� ������������� ������� � ������� � ���� �����
}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableId() == ItemID;} ); // ����� ���� � ������� �� ������������� ���� ������ � �����
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return !Slot.Item.IsValid();} ); // ��������� ������ ����, ��� ����� �� �������
}
