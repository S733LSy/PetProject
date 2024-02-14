// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlotWidget.generated.h"

class UWeaponInventoryItem;
class UInventorySlotWidget;
class AEquipableItem;
class UImage;
class UTextBlock;
UCLASS()
class S733LSYMAINPROJECT_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnEquipmentDropInSlot, const TSubclassOf<AEquipableItem>&, int32); // RetVal - делегат возвращает значение
	//(тип значения, который мы возвращаем, имя делегата, тип первого параметра, тип второго параметра (слот в который пытаемся всё передать))
	DECLARE_DELEGATE_OneParam(FOnEquipmentRemoveFromSlot, int32); // делегат удаления из какого-то определенного слота

	FOnEquipmentDropInSlot OnEquipmentDropInSlot;
	FOnEquipmentRemoveFromSlot OnEquipmentRemoveFromSlot;

	void InitializeEquipmentSlot(TWeakObjectPtr<AEquipableItem> Equipment, int32 Index);
	void UpdateView();
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ImageWeaponIcon; // иконка

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TBWeaponName; // имя экиперуемого объета

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlotWidget> DragAndDropWidgetClass;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override; // обработка нажатия кнопки мыши, при нажатии
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override; // начало операции переноса айтема
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override; // конец переноса
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override; // отмена переноса

private:
	TWeakObjectPtr<AEquipableItem> LinkedEquipableItem;
	TWeakObjectPtr<UWeaponInventoryItem> AdapterLinkedInventoryItem;
	int32 SlotIndexInComponent = 0;
};
