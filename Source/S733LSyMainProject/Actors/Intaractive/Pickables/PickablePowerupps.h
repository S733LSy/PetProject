
#pragma once

#include "CoreMinimal.h"
#include "Actors/Intaractive/Pickables/PickableItem.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "PickablePowerupps.generated.h"


UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API APickablePowerupps : public APickableItem
{
	GENERATED_BODY()

public:
	APickablePowerupps();
	
	virtual void Interact(ASMBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PowerupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemType")
	EPickableItemType Type = EPickableItemType::Weapon;
};
