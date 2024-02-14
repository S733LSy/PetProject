// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSlotWidget.h"
#include "Inventory/Items/InventoryItem.h"
#include "Inventory/Items/Equipables/WeaponInventoryItem.h"
#include "Utils/SMDataTableUtils.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/Widget/Inventory/InventorySlotWidget.h"

void UEquipmentSlotWidget::InitializeEquipmentSlot(TWeakObjectPtr<AEquipableItem> Equipment, int32 Index)
{
	if (!Equipment.IsValid())
	{
		return;
	}
	
	LinkedEquipableItem = Equipment;
	SlotIndexInComponent = Index;

	FWeaponTableRow* EquipmentData = SMDataTableUtils::FindWeaponData(Equipment->GetDataTableID());
	if (EquipmentData != nullptr)
	{
		AdapterLinkedInventoryItem = NewObject<UWeaponInventoryItem>(Equipment->GetOwner());
		AdapterLinkedInventoryItem->Initialize(Equipment->GetDataTableID(), EquipmentData->WeaponItemDescrition);
		AdapterLinkedInventoryItem->SetEquipWeaponClass(EquipmentData->EquipableActor);
	}
}

void UEquipmentSlotWidget::UpdateView()
{
	if (LinkedEquipableItem.IsValid())
	{
		ImageWeaponIcon->SetBrushFromTexture(AdapterLinkedInventoryItem->GetDescription().Icon);
		TBWeaponName->SetText(AdapterLinkedInventoryItem->GetDescription().Name);
	}
	else
	{
		ImageWeaponIcon->SetBrushFromTexture(nullptr);
		TBWeaponName->SetText(FText::FromName(FName(NAME_None)));
	}
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!LinkedEquipableItem.IsValid())
	{
		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	checkf(DragAndDropWidgetClass.Get() != nullptr, TEXT("UEquipmentSlotWidget::NativeOnDragDetected drag and drop widget is not defined")); // определен ли виджет

	if (!AdapterLinkedInventoryItem.IsValid())
	{
		return;
	}

	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass())); // создаём операцию

	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), DragAndDropWidgetClass); // создаём виджет
	DragWidget->SetItemIcon(AdapterLinkedInventoryItem->GetDescription().Icon);

	DragOperation->DefaultDragVisual = DragWidget; // визуал передаём
	DragOperation->Pivot = EDragPivot::CenterCenter; // передаём ось
	DragOperation->Payload = AdapterLinkedInventoryItem.Get(); // полезная нагрузка - объект инвентори айтема
	OutOperation = DragOperation;

	LinkedEquipableItem.Reset(); // очищаем связанный айтем
	OnEquipmentRemoveFromSlot.ExecuteIfBound(SlotIndexInComponent);

	UpdateView();
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UWeaponInventoryItem* OperationObject = Cast<UWeaponInventoryItem>(InOperation->Payload); // пытаемся получить полезную нагрузку
	if (IsValid(OperationObject))
	{
		return OnEquipmentDropInSlot.Execute(OperationObject->GetEquipWeaponClass(), SlotIndexInComponent);
	}
	return false;
}

void UEquipmentSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	AdapterLinkedInventoryItem = Cast<UWeaponInventoryItem>(InOperation->Payload);
	OnEquipmentDropInSlot.Execute(AdapterLinkedInventoryItem->GetEquipWeaponClass(), SlotIndexInComponent);
}
