// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion); // Dynamic for blueprints

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UExplosionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void Explode(AController* Controller); // ����� � ������� �������� � ��������� �����

	UPROPERTY(BlueprintAssignable) // BlueprintAssignable for delegates
	FOnExplosion OnExplosion;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float MaxDamage = 75.0f; // ������������ ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float MinDamage = 30.0f; // ����������� ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float DamageFallOff = 1.0f; // ����������� �������� ����� 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass; // ��� �����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius")
	float InnerRadius = 300.0f; // ���������� ������, � ������� ����� ���������� ������������ ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius")
	float OuterRadius = 500.0f; // ������ �� ������� ����� ���������� ����������� ����

	UPROPERTY(BlueprintReadOnly, Category = "Explosion | VFX")
	UParticleSystem* ExplosionVFX;

private:
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_Explode();
};
