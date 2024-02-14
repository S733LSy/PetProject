// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Runtime\Engine\Classes\Components\TimelineComponent.h"
#include "CoreMinimal.h"
#include "SMBaseCharacter.h"
#include "SMPlayerCharacter.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API ASMPlayerCharacter : public ASMBaseCharacter
{
	GENERATED_BODY()

public:

	ASMPlayerCharacter(const FObjectInitializer& ObjectInitializer); // конструктор, местов котором будут создаваться компоненты

	virtual void MoveForward(float Value) override; // переопределение метода
	virtual void MoveRight(float Value) override; // переопределение метода
	virtual void Turn(float Value) override; // переопределение метода
	virtual void LookUp(float Value) override; // переопределение метода
	virtual void TurnAtRate(float Value) override; // переопределение метода
	virtual void LookUpAtRate(float Value) override; // переопределение метода

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust) override;

	virtual void OnStartProne() override;
	virtual void OnEndProne() override;

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override; // _Implementation потому что это то, что вызывает метод OnJumped(), она есть в блупринтах, но из c++ вызываем _Implementation()

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	FORCEINLINE void BeginSpringArmTimeline() { SpringArmTimeline.Play(); }
	UFUNCTION()
	void UpdateSpringArmTimeline(float Alpha);
	FORCEINLINE void EndSpringArmTimeline() { SpringArmTimeline.Reverse(); }

	void UpdateAimFOVTimeline(float Alpha);

	float SprintSpringArmLength = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | SpringArm")
	UCurveFloat* SpringArmCurveFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Aim")
	UCurveFloat* AimCurveFloat;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
		class UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
		class USpringArmComponent* SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Prone")
		float HeightAdjust = 19.0f;

	virtual void OnStartAimingInternal() override;

	virtual void OnStopAimingInternal() override;

	class ARangeWeaponItem* GetCurrentRangeWeaponItem() const;

private:
	float DefaultSpringArmLength = 300.0f;
	FTimeline SpringArmTimeline;

	FTimeline AimFOVTimeline;

};