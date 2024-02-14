// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S733LSyMainProjectTypes.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWheelWidget.generated.h"

struct FWeaponTableRow;
class UCharacterEquipmentComponent;
class UTextBlock;
class UImage;

UCLASS()
class S733LSYMAINPROJECT_API UWeaponWheelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent);

	void NextSegment();
	void PreviousSegment();
	void ConfirmSelection();

protected:
	virtual void NativeConstruct() override;
	void SelectSegment();

	UPROPERTY(meta = (BindWidget))
	UImage* RadialBackground;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponNameText;

	// array for correspondence between equipment slots and radial menu segments (array indices)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon wheel settings")
	TArray<EEquipmentSlots> EquipmentSlotsSegments;

private:
	FWeaponTableRow* GetTableRowForSegment(int32 SegmentIndex) const; // функция нахождения описания конретного айтема для выбранного сегмента

	int32 CurrentSegmentIndex;
	UMaterialInstanceDynamic* BackgroundMaterial;

	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;
};
