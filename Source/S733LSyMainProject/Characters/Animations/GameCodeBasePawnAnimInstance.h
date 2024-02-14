// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../S733LSyMainProjectBasePawn.h"
#include "GameCodeBasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API UGameCodeBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base pawn animation instance")
	float InputForward;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base pawn animation instance")
		float InputRight;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base pawn animation instance")
		bool bIsInAir;

private:
	TWeakObjectPtr<class AS733LSyMainProjectBasePawn> CachedBasePawn;
};
