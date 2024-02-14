// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Items/Equipables/WeaponInventoryItem.h"
#include "UI/Widget/Equipment/EquipmentSlotWidget.h"
#include "InventorySlotWidget.generated.h"

class UImage;
struct FInventorySlot;

UCLASS()
class S733LSYMAINPROJECT_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeItemSlot(FInventorySlot& InventorySlot); // связь инвентори слота с текущим виджетом и записать данные в LinkedSlot
	void UpdateView(); // обновление представлен используется внутри колбэка нашего инвентори стола виджета
	void SetItemIcon(UTexture2D* Icon);

protected:
	UPROPERTY(meta = (BindWidget)) // Иконка создаётся внутри готового виджет блупринта
	UImage* ImageItemIcon; //

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 Count;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override; // обработка нажатия кнопки мыши, при нажатии
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override; // начало операции переноса айтема
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override; // конец переноса
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override; // отмена переноса

private:
	int32 CountOut;
	FInventorySlot* LinkedSlot;
};
