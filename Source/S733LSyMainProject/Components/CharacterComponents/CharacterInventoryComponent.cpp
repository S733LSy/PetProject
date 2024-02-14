
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

void UCharacterInventoryComponent::OpenViewInventory(APlayerController* PlayerController) // метод открытия инвентаря
{
	if (!IsValid(InventoryViewWidget)) // проверка на валидность виджета инвенторя
	{
		CreateViewWidget(PlayerController); // если не валиден, то создаём его
	}

	if (!InventoryViewWidget->IsVisible()) // проверка на видимость виджета
	{
		InventoryViewWidget->AddToViewport(); // если не виден, то добавляем во вьюпорт
	}
}

void UCharacterInventoryComponent::CloseViewInventory() // метод закрытия инвентаря
{
	if (InventoryViewWidget->IsVisible()) // проверка виджета на видимость
	{
		InventoryViewWidget->RemoveFromParent(); // если он виден, то удаляем его
	}
}

bool UCharacterInventoryComponent::IsViewVisible() const // геттер виден виджет или нет
{
	bool Result = false; // создаём переменную
	if (IsValid(InventoryViewWidget)) // если виджет валиден
	{
		Result = InventoryViewWidget->IsVisible(); // результат будет равен в зависимоти от того, виден виджет или нет
	}
	return Result; // Если не создался виджет инвенторя, то возвращается ложное значение, для этого и была создана переменная Result
}

int32 UCharacterInventoryComponent::GetCapacity() const // геттер количества слотов
{
	return Capacity; // возвращаем количество слотов
}

bool UCharacterInventoryComponent::HasFreeSlot() const
{
	return ItemsInInventory < Capacity; // возвращает true, если количество предметов в инвенторе меньше чем слотов, в остлальных случаях false
}

void UCharacterInventoryComponent::AddItemToFreeSlot(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, bool& Result)
{
	FInventorySlot* FreeSlot = FindFreeSlot(); // Ищём свободный слот
	if (FreeSlot != nullptr) // если свободный слот не равен нулевой ссылке
	{ // Далее работаем со свободным слотом, если он есть
		FreeSlot->Item = ItemToAdd; // У слота, который является ссылкой на структуру присваиваем айтем на добавление
		FreeSlot->Count = Count; // Также передаём значение в количество у данного айтема
		ItemsInInventory++; // К количеству предметов в инвентаре добавляем 1, это количество занятых слотов
		Result = true; // результат добавления делаем тру
		FreeSlot->UpdateSlotState(); // у свободного слота вызываем метод апдейта
	}
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count, EPickableItemType ItemType)
{
	if (!ItemToAdd.IsValid() || Count < 0) // если айтем на добавление не валиден или его количество меньше нуля
	{
		return false; // то добавление происходить не будет
	}

	bool Result = false; // Создаём переменную - результат добавления
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

	for (FInventorySlot& Slot : InventorySlots) // роходимя по всему массиву слотов
	{
		if (Slot.Item.IsValid()) // если слот валиден
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
	FInventorySlot* ItemSlot = FindItemSlot(ItemID); // ищем слот с айди, которое передаётся в метод
	if (ItemSlot != nullptr) // если существует
	{
		InventorySlots.RemoveAll([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableId() == ItemID; }); // даляем только те слоты, которые равны ItemID
		return true; // возращаем тру, если удалили
	}
	return false; // false, если не удалили
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FText> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FText> Result; // массив с результатов
	for (const FInventorySlot& Slot : InventorySlots) // роходимя по всему массиву слотов
	{
		if (Slot.Item.IsValid()) // если слот валиден
		{
			Result.Add(Slot.Item->GetDescription().Name); // то в массив результата добавляем имя этого айтема
		}
	}
	return Result;
}

void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InventorySlots.AddDefaulted(Capacity); // езервация элементов массива айтемов по количеству возможных слотов
}

void UCharacterInventoryComponent::CreateViewWidget(APlayerController* PlayerController)
{
	if (IsValid(InventoryViewWidget)) // если валиден виджет, то выходим, зачем его заново создавать
	{
		return;
	}
	
	if (!IsValid(PlayerController) || !IsValid(InventoryViewWidgetClass)) // если не валиден контроллер игрока или не валиден класс виджета
	{
		return; // то выходим из метода
	}

	InventoryViewWidget = CreateWidget<UInventoryViewWidget>(PlayerController, InventoryViewWidgetClass); // создаём виджет
	InventoryViewWidget->InitializeViewWidget(InventorySlots); // вызываем метод инициализации виджета и передаём в него слоты
}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableId() == ItemID;} ); // Наход слот с айтемом по передаваемому айди айтема в метод
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return !Slot.Item.IsValid();} ); // возращаем первый слот, где айтем не валиден
}
