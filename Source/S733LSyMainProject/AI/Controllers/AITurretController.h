
#pragma once

#include "CoreMinimal.h"
#include "SMAIController.h"
#include "AITurretController.generated.h"

class ATurret;
UCLASS()
class S733LSYMAINPROJECT_API AAITurretController : public ASMAIController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

protected:
	UPROPERTY(EditAnyWhere, BlueprintREadOnly, Category = "Turret parameters | Max firing distance")
	float MaxFiringDistanceRadius = 500.0f;

	void DeathEvent();

private:
	TWeakObjectPtr<ATurret> CachedTurret;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
