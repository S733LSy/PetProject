// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MPBaseCharacterAniminstance.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API UMPBaseCharacterAniminstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		float Speed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsFalling = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsCrouching = false;

private:
	TWeakObjectPtr<class AMPBaseCharacter> ChachedBaseCharacter;
};
