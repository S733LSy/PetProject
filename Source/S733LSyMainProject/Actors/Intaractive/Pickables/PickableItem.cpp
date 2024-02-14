
#include "PickableItem.h"

const FName& APickableItem::GetDataTableID() const
{
	return DataTableID;
}

void APickableItem::SetCount(int32 Count_In)
{
	Count = Count_In;
}

int32 APickableItem::GetCount() const
{
	return Count;
}
