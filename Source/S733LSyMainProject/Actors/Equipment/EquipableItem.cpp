#include "EquipableItem.h"
#include "Characters/SMBaseCharacter.h"

AEquipableItem::AEquipableItem()
{
	SetReplicates(true);
}

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<ASMBaseCharacter>(), TEXT("AEquipableItem::SetOwner() only character can be an owner of an equipable item"));
		CachedCharacterOwner = StaticCast<ASMBaseCharacter*>(GetOwner());
		if (GetLocalRole() == ROLE_Authority)
		{
			SetAutonomousProxy(true); // в зависимости от владельца вызовет данный метод на нужной машине
		}
	}
	else
	{
		CachedCharacterOwner = nullptr;
	}
}

ASMBaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedCharacterOwner.IsValid() ? CachedCharacterOwner.Get() : nullptr;
}

EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::UnEquip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

FName AEquipableItem::GetDataTableID() const
{
	return DataTableID;
}

bool AEquipableItem::IsSlotCompatable(EEquipmentSlots Slot)
{
	return CompatableEquipmentSlots.Contains(Slot);
}
