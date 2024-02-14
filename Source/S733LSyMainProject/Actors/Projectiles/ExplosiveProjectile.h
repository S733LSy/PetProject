
#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "ExplosiveProjectile.generated.h"

class UExplosionComponent;
UCLASS()
class S733LSYMAINPROJECT_API AExplosiveProjectile : public ASMProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 2.0f; // �������� ����� ������� ������� � �������

	virtual void OnProjectileLaunched() override;

private:
	void OnDetonationTimerElapsed(); // �����, ������� ����������, ����� ������ �������� � �����

	AController* GetController() const;

	FTimerHandle DetonationTimer;
};
