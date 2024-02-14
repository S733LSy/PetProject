// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMProjectile.generated.h"
class ASMProjectile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, ASMProjectile*, Projectile, const FHitResult&, Hit, const FVector&, Direction);

UCLASS()
class S733LSYMAINPROJECT_API ASMProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction); // the launch method, the direction is transmitted

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHitEvent;

	UFUNCTION(BlueprintNativeEvent)
	void SetProjectileActive(bool bIsProjectileActive);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent; // Collision component for projectile

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void BeginPlay() override;

	virtual void OnProjectileLaunched();

private:
	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
