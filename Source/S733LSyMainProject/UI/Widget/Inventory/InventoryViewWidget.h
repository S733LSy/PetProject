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
	void InitializeViewWidget(TArray<FInventorySlot>& InventorySlots); // �������������, ��������� �� ���� ������ ������ ����������� � �������� ����� AddItemSlotView

protected:
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridPanelItemSlots; // �������� ������

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass; // �������� ����� �������, ��� ������, ������� �� ������

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	int32 Column = 4; // ������� �������

	void AddItemSlotView(FInventorySlot& SlotToAdd); // ��������� ��������� ���� �������, ������� �� �������
};
