// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S733LSyMainProjectTypes.h"
#include "Actors/Intaractive/Pickables/PickableItem.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "PickableAmmo.generated.h"

struct FAmmoTableRow;

UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API APickableAmmo : public APickableItem
{
	GENERATED_BODY()

public:
	APickableAmmo();

	virtual void Interact(ASMBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo | Count")
	int32 PickableAmmoCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Type")
	EPickableItemType Type = EPickableItemType::Ammo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Type")
	EAmunitionType AmmoType = EAmunitionType::None;
};
