// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOutCharacterOxygenEventSignature);
DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature); // создаём делегат - это тип переменной
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaEventSignature, bool);
DECLARE_MULTICAST_DELEGATE(FOnStaminaUpdate);
DECLARE_MULTICAST_DELEGATE(FOnOxygenUpdate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributeComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent; // Создаём переменную типа делегата FOnDeathEventSignature

	FOutCharacterOxygenEventSignature OutOfOxygen;

	FOnHealthChanged OnHealthChangedEvent;

	bool IsAlive() { return Health > 0.0f; }

	FOutOfStaminaEventSignature OutOfStaminaEvent;

	FOnStaminaUpdate OnStaminaUpdate;
	FOnOxygenUpdate OnOxygenUpdateEvent;

	float GetHealthPercent() const;
	float GetStaminaPercent() const;
	float GetOxygenPercent() const;

	void AddHealth(float HealthToAdd);
	void RestoreFullStamina();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Stamina")
	float MaxStamina = 100.0f; // максимальное значение выносливости игрока
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Stamina")
	float StaminaRestoreVelocity = 25.0f; // Скорость восстановления выносливости
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Stamina")
	float SprintStaminaConsumptionVelocity = 25.f; // Скорость потребеления выносливости игрока
	void UpdateStaminaValue(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaterVolume | Oxygen")
	float MaxOxygen = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaterVolume | Oxygen")
	float OxygenRestoreVelocity = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaterVolume | Oxygen")
	float SwimOxygenConsumptionVelocity = 2.0f;
	void UpdateOxygenValue(float DeltaTime);

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health = 0.0f;
	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	float Stamina = 0.0f;
	float Oxygen = 0.0f;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser); // параметры из Actor.h
	// DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_FiveParams( FTakeAnyDamageSignature, AActor, OnTakeAnyDamage, AActor*, DamagedActor, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser );

	TWeakObjectPtr<class ASMBaseCharacter> CachedBaseCharacterOwner;
};
