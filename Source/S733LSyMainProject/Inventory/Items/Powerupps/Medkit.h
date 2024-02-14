
#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/InventoryItem.h"
#include "Medkit.generated.h"


UCLASS()
class S733LSYMAINPROJECT_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()

public:
	virtual bool Consume(ASMBaseCharacter* ConsumeTarget) override; // метод потребления из инвенторя
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Medkit")
	float Health = 25.0f;
	
};
