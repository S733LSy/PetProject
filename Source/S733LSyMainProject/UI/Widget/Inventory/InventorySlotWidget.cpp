
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
	LinkedSlot = &InventorySlot; // помещаем в указатель на слот адрес слота инвенторя

	FInventorySlot::FInventorySlotUpdate OnInventorySlotUpdate; // Создаём делегат, локальный коллбэк
	OnInventorySlotUpdate.BindUObject(this, &UInventorySlotWidget::UpdateView); // привязываем метод на этот делегат
	LinkedSlot->BindOnInventorySlotUpdate(OnInventorySlotUpdate); // привязываем к слоту
}

void UInventorySlotWidget::UpdateView()
{
	if (LinkedSlot == nullptr) // если пустой, то выходим
	{
		ImageItemIcon->SetBrushFromTexture(nullptr); // задаём иконке нулл птр, имя переменной переносится в блупринты, а иконка биндится из кода
		return;
	}

	if (LinkedSlot->Item.IsValid())
	{
		const FInventoryItemDescription& Description = LinkedSlot->Item->GetDescription(); // получаем описание айтема
		ImageItemIcon->SetBrushFromTexture(Description.Icon); // задаём иконку
		Count = LinkedSlot->Count;
	}
	else
	{
		ImageItemIcon->SetBrushFromTexture(nullptr); // задаём иконке нулл птр
		Count = 0;
	}
}

void UInventorySlotWidget::SetItemIcon(UTexture2D* Icon)
{
	ImageItemIcon->SetBrushFromTexture(Icon);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LinkedSlot == nullptr) // если не привязан никакой слот
	{
		return FReply::Handled(); // вещь обработана
	}
	if (!LinkedSlot->Item.IsValid()) // если айтем не валиден
	{
		return FReply::Handled(); // успешно обработали
	}

	FKey MouseBtn = InMouseEvent.GetEffectingButton(); // Получение кнопки мыши
	if (MouseBtn == EKeys::RightMouseButton) // Если правая кнопка мыши (можно сделать в свойствах, чтобы был выбор у гейм дизайнера)
	{
		TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item; // Находим айтем из линкед слота
		ASMBaseCharacter* ItemOwner = Cast<ASMBaseCharacter>(LinkedSlotItem->GetOuter()); // получаем владельца для любого UObject и получаем из него нашего персонажа

		if (LinkedSlotItem->Consume(ItemOwner)) // пробуем употребить данный айтем
		{
			LinkedSlot->ClearSlot(); // если успешно, то очищаем слот
		}
		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton); // используем стандартный ответ на перенос
	return Reply.NativeReply; // возвращаем nativereply от данного ответа
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass())); // создаём операция перенесения

	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass()); // создание нового инвентори слота виджета
	DragWidget->ImageItemIcon->SetBrushFromTexture(LinkedSlot->Item->GetDescription().Icon); // выставляем иконку

	DragOperation->DefaultDragVisual = DragWidget; // выставляем визуал
	DragOperation->Pivot = EDragPivot::MouseDown; // выставляем ось
	DragOperation->Payload = LinkedSlot->Item.Get(); // выставляем полезную нагрузку, у нас это сам айтем
	OutOperation = DragOperation; // записываем операцию перемещения в выходную операцию

	CountOut = LinkedSlot->Count;

	LinkedSlot->ClearSlot(); // очищаем слот виджета
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!LinkedSlot->Item.IsValid()) // если слот пустой
	{
		LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload)); // записываем новый виджет из пэйлоад
		LinkedSlot->Count = CountOut;
		CountOut = 0;
		LinkedSlot->UpdateSlotState(); // обновляем состояние данного слота
		return true; // возвращаем успех
	}
	return false; // возвращаем неуспешно
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload)); // возвращаем элемент айтема из которого сделали и текущий элемент перемещения (InOperation->Payload)
	TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item; // Находим айтем из линкед слота
	ASMBaseCharacter* ItemOwner = Cast<ASMBaseCharacter>(LinkedSlotItem->GetOuter()); // получаем владельца для любого UObject и получаем из него нашего персонажа
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
