
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentViewWidget.generated.h"

class UCharacterEquipmentComponent;
class UEquipmentSlotWidget;
class UVerticalBox;
class AEquipableItem;

UCLASS()
class S733LSYMAINPROJECT_API UEquipmentViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeEquipmentWidget(UCharacterEquipmentComponent* EquipmentComponent); // нициализация виджета

protected:
	void AddEquipmentSlotView(AEquipableItem* LinkToWeapon, int32 SlotIndex); // добавление вьюхи в слот
	void UpdateSlot(int32 SlotIndex); // обновление слота

	bool EquipEquipmentToSlot(const TSubclassOf<AEquipableItem>& WeaponClass, int32 SenderIndex); // метод экипировки
	void RemoveEquipmentFromSlot(int32 SlotIndex); // удаление элемента из слота

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VBWeaponSlots;

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	TSubclassOf<UEquipmentSlotWidget> DefaultSlotViewClass;

	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;
};
