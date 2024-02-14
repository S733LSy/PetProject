
#include "InventorySlotWidget.h"

#include "Actors/Intaractive/Pickables/PickableItem.h"
#include "Actors/Intaractive/Pickables/PickableWeapon.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Inventory/Items/InventoryItem.h"
#include "Characters/SMBaseCharacter.h"
#include "Utils/SMDataTableUtils.h"

void UInventorySlotWidget::InitializeItemSlot(FInventorySlot& InventorySlot)
{
	LinkedSlot = &InventorySlot; // �������� � ��������� �� ���� ����� ����� ���������

	FInventorySlot::FInventorySlotUpdate OnInventorySlotUpdate; // ������ �������, ��������� �������
	OnInventorySlotUpdate.BindUObject(this, &UInventorySlotWidget::UpdateView); // ����������� ����� �� ���� �������
	LinkedSlot->BindOnInventorySlotUpdate(OnInventorySlotUpdate); // ����������� � �����
}

void UInventorySlotWidget::UpdateView()
{
	if (LinkedSlot == nullptr) // ���� ������, �� �������
	{
		ImageItemIcon->SetBrushFromTexture(nullptr); // ����� ������ ���� ���, ��� ���������� ����������� � ���������, � ������ �������� �� ����
		return;
	}

	if (LinkedSlot->Item.IsValid())
	{
		const FInventoryItemDescription& Description = LinkedSlot->Item->GetDescription(); // �������� �������� ������
		ImageItemIcon->SetBrushFromTexture(Description.Icon); // ����� ������
		Count = LinkedSlot->Count;
	}
	else
	{
		ImageItemIcon->SetBrushFromTexture(nullptr); // ����� ������ ���� ���
		Count = 0;
	}
}

void UInventorySlotWidget::SetItemIcon(UTexture2D* Icon)
{
	ImageItemIcon->SetBrushFromTexture(Icon);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LinkedSlot == nullptr) // ���� �� �������� ������� ����
	{
		return FReply::Handled(); // ���� ����������
	}
	if (!LinkedSlot->Item.IsValid()) // ���� ����� �� �������
	{
		return FReply::Handled(); // ������� ����������
	}

	FKey MouseBtn = InMouseEvent.GetEffectingButton(); // ��������� ������ ����
	if (MouseBtn == EKeys::RightMouseButton) // ���� ������ ������ ���� (����� ������� � ���������, ����� ��� ����� � ���� ���������)
	{
		TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item; // ������� ����� �� ������ �����
		ASMBaseCharacter* ItemOwner = Cast<ASMBaseCharacter>(LinkedSlotItem->GetOuter()); // �������� ��������� ��� ������ UObject � �������� �� ���� ������ ���������

		if (LinkedSlotItem->Consume(ItemOwner)) // ������� ���������� ������ �����
		{
			LinkedSlot->ClearSlot(); // ���� �������, �� ������� ����
		}
		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton); // ���������� ����������� ����� �� �������
	return Reply.NativeReply; // ���������� nativereply �� ������� ������
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass())); // ������ �������� �����������

	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass()); // �������� ������ ��������� ����� �������
	DragWidget->ImageItemIcon->SetBrushFromTexture(LinkedSlot->Item->GetDescription().Icon); // ���������� ������

	DragOperation->DefaultDragVisual = DragWidget; // ���������� ������
	DragOperation->Pivot = EDragPivot::MouseDown; // ���������� ���
	DragOperation->Payload = LinkedSlot->Item.Get(); // ���������� �������� ��������, � ��� ��� ��� �����
	OutOperation = DragOperation; // ���������� �������� ����������� � �������� ��������

	CountOut = LinkedSlot->Count;

	LinkedSlot->ClearSlot(); // ������� ���� �������
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!LinkedSlot->Item.IsValid()) // ���� ���� ������
	{
		LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload)); // ���������� ����� ������ �� �������
		LinkedSlot->Count = CountOut;
		CountOut = 0;
		LinkedSlot->UpdateSlotState(); // ��������� ��������� ������� �����
		return true; // ���������� �����
	}
	return false; // ���������� ���������
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload)); // ���������� ������� ������ �� �������� ������� � ������� ������� ����������� (InOperation->Payload)
	TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item; // ������� ����� �� ������ �����
	ASMBaseCharacter* ItemOwner = Cast<ASMBaseCharacter>(LinkedSlotItem->GetOuter()); // �������� ��������� ��� ������ UObject � �������� �� ���� ������ ���������
	FVector SpawnItemLocation = ItemOwner->GetActorLocation() + FVector(30.0f, 0.0f, -30.0f);
	FRotator SpawnItemRotator = ItemOwner->GetActorRotation();
	FAmmoTableRow* AmmoRow = SMDataTableUtils::FindInventoryAmmoData(LinkedSlot->Item->GetDataTableId());
	FWeaponTableRow* WeaponTableRow = SMDataTableUtils::FindWeaponData(LinkedSlot->Item->GetDataTableId());
	FItemTableRow* ItemTableRow = SMDataTableUtils::FindInventoryItemData(LinkedSlot->Item->GetDataTableId());
	if (AmmoRow != nullptr)
	{
		APickableItem* Item = GetWorld()->SpawnActor<APickableItem>(AmmoRow->PickableActorClass, SpawnItemLocation, SpawnItemRotator);
		Item->SetCount(CountOut);
		CountOut = 0;
	}
	if (WeaponTableRow != nullptr)
	{
		GetWorld()->SpawnActor<APickableWeapon>(WeaponTableRow->PickableActor, SpawnItemLocation, SpawnItemRotator);
	}
	if (ItemTableRow != nullptr)
	{
		GetWorld()->SpawnActor<APickableItem>(ItemTableRow->PickableActorClass, SpawnItemLocation, SpawnItemRotator);
	}

	LinkedSlot->ClearSlot();
}
