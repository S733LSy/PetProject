// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryViewWidget.generated.h"

struct FInventorySlot;
class UInventorySlotWidget;
class UGridPanel;

UCLASS()
class S733LSYMAINPROJECT_API UInventoryViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeViewWidget(TArray<FInventorySlot>& InventorySlots); // инициализация, принимает на вход массив слотов инвентарных и вызывает метод AddItemSlotView

protected:
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridPanelItemSlots; // сетчетая панель

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass; // подкласс слота виджета, тот объект, который мы создаём

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	int32 Column = 4; // счётчик колонок

	void AddItemSlotView(FInventorySlot& SlotToAdd); // добавляет инвентори слот виджеты, которые мы создали
};
