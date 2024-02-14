
#include "EquipmentViewWidget.h"
#include "EquipmentSlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/EquipableItem.h"

void UEquipmentViewWidget::InitializeEquipmentWidget(UCharacterEquipmentComponent* EquipmentComponent)
{
	LinkedEquipmentComponent = EquipmentComponent; // записываем эквипмент компонент
	const TArray<AEquipableItem*>& Items = LinkedEquipmentComponent->GetItems(); // ссылка на элементы из компонента
	for (int32 Index = 1; Index < Items.Num(); ++Index) // пропускаем пустой слот
	{
		AddEquipmentSlotView(Items[Index], Index); // для каждого элемента создаём вьюху для каждого слота
	}
}

void UEquipmentViewWidget::AddEquipmentSlotView(AEquipableItem* LinkToWeapon, int32 SlotIndex)
{
	checkf(IsValid(DefaultSlotViewClass.Get()), TEXT("UEquipmentViewWidget::AddEquipmentSlotView equipment slot widget is not set"));

	UEquipmentSlotWidget* SlotWidget = CreateWidget<UEquipmentSlotWidget>(this, DefaultSlotViewClass);

	if (IsValid(SlotWidget))
	{
		SlotWidget->InitializeEquipmentSlot(LinkToWeapon, SlotIndex);

		VBWeaponSlots->AddChildToVerticalBox(SlotWidget);
		SlotWidget->UpdateView();
		SlotWidget->OnEquipmentDropInSlot.BindUObject(this, &UEquipmentViewWidget::EquipEquipmentToSlot);
		SlotWidget->OnEquipmentRemoveFromSlot.BindUObject(this, &UEquipmentViewWidget::RemoveEquipmentFromSlot);
	}
}

void UEquipmentViewWidget::UpdateSlot(int32 SlotIndex)
{
	UEquipmentSlotWidget* WidgetToUpdate = Cast<UEquipmentSlotWidget>(VBWeaponSlots->GetChildAt(SlotIndex - 1)); // из-за none слота передаём на ё меньше
	if (IsValid(WidgetToUpdate))
	{
		WidgetToUpdate->InitializeEquipmentSlot(LinkedEquipmentComponent->GetItems()[SlotIndex], SlotIndex);
		WidgetToUpdate->UpdateView();
	}
}

bool UEquipmentViewWidget::EquipEquipmentToSlot(const TSubclassOf<AEquipableItem>& WeaponClass, int32 SenderIndex)
{
	const bool Result = LinkedEquipmentComponent->AddEquipmentItemToSlot(WeaponClass, SenderIndex);
	if (Result)
	{
		UpdateSlot(SenderIndex);
	}
	return Result;
}

void UEquipmentViewWidget::RemoveEquipmentFromSlot(int32 SlotIndex)
{
	LinkedEquipmentComponent->RemoveItemFromSlot(SlotIndex);
}
