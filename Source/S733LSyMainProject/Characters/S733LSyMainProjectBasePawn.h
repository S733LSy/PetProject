// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "S733LSyMainProjectBasePawn.generated.h"

UCLASS()
class S733LSYMAINPROJECT_API AS733LSyMainProjectBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AS733LSyMainProjectBasePawn();


	UPROPERTY(VisibleAnywhere)
	class UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Jump();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputForward() { return InputForward; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputRight() { return InputRight; }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBlendComplete();

	float InputForward = 0.0f;
	float InputRight = 0.0f;

	AActor* CurrentViewActor;
};
