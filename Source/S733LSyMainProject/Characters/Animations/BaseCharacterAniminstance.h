// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPBaseCharacterAniminstance.h"
#include "BaseCharacterAniminstance.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API UBaseCharacterAniminstance : public UMPBaseCharacterAniminstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK settings")
		FVector RightFootEffectorLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK settings")
		FVector LeftFootEffectorLocation = FVector::ZeroVector;
	
private:
	TWeakObjectPtr<class APlayerCharacter> ChachedPlayerCharacterOwner;
};
