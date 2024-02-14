// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S733LSyMainProjectTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class UWeaponWheelWidget;
class UEquipmentViewWidget;
typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentThrowableItemAmmoChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

class AThrowableItem;
class ARangeWeaponItem;
class AMeleeWeaponItem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API UCharacterEquipmentComponent : public UActorComponent // Класс которой позволяет экипировать предмет
{
	GENERATED_BODY()

public:
	UCharacterEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;

	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;

	bool IsEquipping() const;

	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnCurrentThrowableItemAmmoChanged OnCurrentThrowableItemAmmoChangedEvent;
	FOnEquippedItemChanged OnEquippedItemChangedEvent;

	UFUNCTION(Server, Reliable)
	void ReloadCurrentWeapon();

	void UnEquipCurrentItem();
	void AttachCurrentItemToEquippedSocket();

	void LaunchCurrentThrowableItem();

	void EquipItemInSlot(EEquipmentSlots Slot);

	void EquipNextItem();
	void EquipPreviousItem();

	int32 GetCurrentThrowableAmmo() const;

	bool AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex);
	void RemoveItemFromSlot(int32 SlotIndex);

	void OpenViewEquipment(APlayerController* PlayerController);
	void CloseViewEquipment();
	bool IsViewVisible() const;

	void OpenWeaponWheel(APlayerController* PlayerController);
	bool IsSelectingWeapon() const;
	void ConfirmWeaponSelection() const;

	const TArray<AEquipableItem*>& GetItems() const; // & - ссылка на массив (на сам объект, не его копия)

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount; // Максимальное количество амуниции

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout; // Collection

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching; // Коллекция уникальных значений, нельзя добавить что-то больше 2 раз

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View")
	TSubclassOf<UEquipmentViewWidget> ViewWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View")
	TSubclassOf<UWeaponWheelWidget> WeaponWheelWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Items")
	TMap<EAmunitionType, FName> AmmoLoadout;

	void CreateEquipmentWidgets(APlayerController* PlayerController);
	
private:
	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot);

	UPROPERTY(Replicated)
	TArray<int32> AmunitionArray;
	UPROPERTY(ReplicatedUsing=OnRep_ItemsArray)
	TArray<AEquipableItem*> ItemsArray;
	UFUNCTION()
	void OnRep_ItemsArray();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_Reload();

	UFUNCTION()
	void OnWeaponReloadComplete();

	bool CanEquipThrowableItem(EEquipmentSlots Slot);

	void CreateLoadout(); // метод, в котором крепится оружие

	void AutoEquip();

	void EquipAnimationFinished();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	int32 GetAvailableAmunitionForCurrentWeapon();

	bool bIsEquipping = false;

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnCurrentThrowableItemAmmoChanged();
	

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;
	FDelegateHandle OnCurrentItemAmmoChangedHandle;

	EEquipmentSlots PreviousEquippedSlot;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquippedSlot)
	EEquipmentSlots CurrentEquippedSlot;
	UFUNCTION()
	void OnRep_CurrentEquippedSlot(EEquipmentSlots CurrentEquippedSlot_Old);


	AEquipableItem* CurrentEquippedItem;
	ARangeWeaponItem* CurrentEquippedWeapon; // текущий предмет, переменная класса "Оружие дальнего боя"
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeaponItem* CurrentMeleeWeapon;

	TWeakObjectPtr<class ASMBaseCharacter> CachedBaseCharacter;
	FTimerHandle EquipTimer;

	UEquipmentViewWidget* ViewWidget;
	UWeaponWheelWidget* WeaponWheelWidget;
};
