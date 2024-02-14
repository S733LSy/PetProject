// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	AThrowableItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void Throw(); // Throw

	virtual void BeginPlay() override;

	EAmunitionType GetAmmoType() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowablePool", meta = (UIMin = 1, ClampMin = 1))
	int32 ThrowablePoolSize = 10;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class ASMProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.0f, UIMax = 90.0f, ClampMin = -90.0f, ClampMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
	EAmunitionType AmmoType;

private:
	UPROPERTY(Replicated)
	TArray<ASMProjectile*> ThrowableItemsPool;
	UPROPERTY(Replicated)
	int32 CurrentThrowableItemIndex;

	const FVector ThrowablePoolLocation = FVector(0.0f, 0.0f, -100.0f); // location of throwable items pool
};
