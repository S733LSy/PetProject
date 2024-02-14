
#include "InventoryViewWidget.h"
#include "InventorySlotWidget.h"
#include "Components/GridPanel.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"

void UInventoryViewWidget::InitializeViewWidget(TArray<FInventorySlot>& InventorySlots)
{
	for (FInventorySlot& Item : InventorySlots) // проходимся по массиву
	{
		AddItemSlotView(Item); // добавляем 
	}
}

void UInventoryViewWidget::AddItemSlotView(FInventorySlot& SlotToAdd)
{
	checkf(InventorySlotWidgetClass.Get() != nullptr, TEXT("UInventoryViewWidget::AddItemSlotView widget class")); // проверка на то, что класс виджета инвенторя не пустой

	UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass); // создаём новый инвентарный виджет (this - владелец, InventorySlotWidgetClass - шаблон)
	if (SlotWidget != nullptr)
	{
		SlotWidget->InitializeItemSlot(SlotToAdd); // Инициализируем

		const int32 CurrentSlotCount = GridPanelItemSlots->GetChildrenCount(); // берем количество элементов грид панели
		const int32 CurrentSlotRow = CurrentSlotCount / Column; // получаем текущий ряд
		const int32 CurrentSlotColumn = CurrentSlotCount % Column; // число колонок слота (остаток от деления)
		GridPanelItemSlots->AddChildToGrid(SlotWidget, CurrentSlotRow, CurrentSlotColumn); // добавление виджета к грид панели

		SlotWidget->UpdateView(); // апдейт
	}
}
