// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPBaseCharacter.generated.h"

UCLASS(Abstract, NotBlueprintable)
class S733LSYMAINPROJECT_API AMPBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMPBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};

	virtual void ChangeCrouchState();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float BaseTurnRate = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float BaseLookUpRate = 45.0f;

};
