#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S733LSyMainProjectTypes.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;
class ASMBaseCharacter;
UCLASS(Abstract, NotBlueprintable)
class S733LSYMAINPROJECT_API AEquipableItem : public AActor // класс итемов, которые может экипировать персонаж
{
	GENERATED_BODY()

public:
	AEquipableItem();

	virtual void SetOwner(AActor* NewOwner) override;

	EEquipableItemType GetItemType() const;

	UAnimMontage* GetCharacterEquipAnimMontage() const;

	FName GetUnEquippedSocketName() const;

	FName GetEquippedSocketName() const;

	virtual void Equip();
	virtual void UnEquip();

	virtual EReticleType GetReticleType() const;

	FName GetDataTableID() const;

	bool IsSlotCompatable(EEquipmentSlots Slot);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item") // Свойство, которое выставляется в пистолете
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item") // итем не прикреплён ни к чему
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item") // итем прикреплён ни к чему
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	TArray<EEquipmentSlots> CompatableEquipmentSlots; // слоты куда мы можем добавить наше оружие, чтобы нельзя было в слот для пистолет добавить миниган

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType ReticleType = EReticleType::None
	;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	ASMBaseCharacter* GetCharacterOwner() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName DataTableID = NAME_None;
	
private:
	TWeakObjectPtr<ASMBaseCharacter> CachedCharacterOwner;
};
