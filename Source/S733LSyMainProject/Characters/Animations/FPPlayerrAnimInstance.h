// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMBaseCharacterAnimInstance.h"
#include "FPPlayerrAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API UFPPlayerrAnimInstance : public USMBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Animations | FirstPerson")
		float PlayerCameraPitchAngle = 0.0f;

	TWeakObjectPtr<class AFPSMPlayerCharacter> CachedFirstPersonCharacterOwner;

private:

	float CalculateCameraPitchAngle() const;
};
