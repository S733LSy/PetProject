// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "S733LSyMainProjectTypes.h"
#include "MeleeWeaponItem.generated.h"

USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee attack")
	TSubclassOf<class UDamageType> DamageTypeClass; // указатель на тип урона, который используетс€ при нанесении урона

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee attack", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float DamageAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee attack")
	class UAnimMontage* AttackMontage;
};

class UMeleeHitRegistrator;
UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API AMeleeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	AMeleeWeaponItem();

	void StartAttack(EMeleeAttackTypes AttackType);

	void SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee attack")
	TMap<EMeleeAttackTypes, FMeleeAttackDescription> Attacks; // ћапа ключ, значение

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& HitDirection);

	TArray<UMeleeHitRegistrator*> HitRegistrators; // ћассив попаданий
	TSet<AActor*> HitActors; // ћножество экторов по которым попал персонаж, набор уникальных значений

	FMeleeAttackDescription* CurrentAttack;
	void OnAttackTimerElapsed();
	FTimerHandle AttackTimer;
};
