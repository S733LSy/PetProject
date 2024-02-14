// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPBaseCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API APlayerCharacter : public AMPBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base character")
		class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
		class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
		class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CHaracter | IK settings")
		FName RightFootSocketName;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CHaracter | IK settings")
		FName LeftFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CHaracter | IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceExtandDistance = 30.0f;

private:
	float GetIKOffsetForASocket(const FName& SocketName);
	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKTraceDistance = 88.0f;

};
