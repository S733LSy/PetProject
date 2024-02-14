
#include "InventoryViewWidget.h"
#include "InventorySlotWidget.h"
#include "Components/GridPanel.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"

void UInventoryViewWidget::InitializeViewWidget(TArray<FInventorySlot>& InventorySlots)
{
	for (FInventorySlot& Item : InventorySlots) // ���������� �� �������
	{
		AddItemSlotView(Item); // ��������� 
	}
}

void UInventoryViewWidget::AddItemSlotView(FInventorySlot& SlotToAdd)
{
	checkf(InventorySlotWidgetClass.Get() != nullptr, TEXT("UInventoryViewWidget::AddItemSlotView widget class")); // �������� �� ��, ��� ����� ������� ��������� �� ������

	UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass); // ������ ����� ����������� ������ (this - ��������, InventorySlotWidgetClass - ������)
	if (SlotWidget != nullptr)
	{
		SlotWidget->InitializeItemSlot(SlotToAdd); // ��������������

		const int32 CurrentSlotCount = GridPanelItemSlots->GetChildrenCount(); // ����� ���������� ��������� ���� ������
		const int32 CurrentSlotRow = CurrentSlotCount / Column; // �������� ������� ���
		const int32 CurrentSlotColumn = CurrentSlotCount % Column; // ����� ������� ����� (������� �� �������)
		GridPanelItemSlots->AddChildToGrid(SlotWidget, CurrentSlotRow, CurrentSlotColumn); // ���������� ������� � ���� ������

		SlotWidget->UpdateView(); // ������
	}
}
