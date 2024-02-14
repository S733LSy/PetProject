// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickableAmmo.h"
#include "Actors/Intaractive/Pickables/PickableItem.h"
#include "PickableWeapon.generated.h"

UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API APickableWeapon : public APickableItem
{
	GENERATED_BODY()

public:
	APickableWeapon();

	virtual void Interact(ASMBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemType")
	EPickableItemType Type = EPickableItemType::Weapon;
	
};
