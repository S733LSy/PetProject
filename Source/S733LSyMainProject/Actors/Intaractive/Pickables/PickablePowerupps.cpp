
#include "PickablePowerupps.h"

#include "S733LSyMainProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/SMDataTableUtils.h"
#include "Characters/SMBaseCharacter.h"

APickablePowerupps::APickablePowerupps()
{
	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));
	SetRootComponent(PowerupMesh);
}

void APickablePowerupps::Interact(ASMBaseCharacter* Character)
{
	FItemTableRow* ItemData = SMDataTableUtils::FindInventoryItemData(GetDataTableID()); // ���������� � �������� � ������� ����� ����

	if (ItemData == nullptr)
	{
		return;
	}

	TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventoryItemClass)); // ������ ��������� ����� � ������ ��������� �� ����, ����� � ������ ���� ����� ���� ��� �������� �������
	Item->Initialize(DataTableID, ItemData->InventoryItemDescription);

	const bool bPickedUp = Character->PickupItem(Item, 1, Type);
	if (bPickedUp)
	{
		Destroy();
	}
}

FName APickablePowerupps::GetActionEventName() const
{
	return ActionInteract;
}
