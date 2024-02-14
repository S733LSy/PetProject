// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacter.h"
#include "FPSMPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API AFPSMPlayerCharacter : public ASMPlayerCharacter
{
	GENERATED_BODY()

public:
	AFPSMPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;
	

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class USkeletalMeshComponent* FirstPersonMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class UCameraComponent* FirstPersonCameraComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float LadderCameraMinPitch = -50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float LadderCameraMaxPitch = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float LadderCameraMinYaw = -30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float LadderCameraMaxYaw = 30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMinPitch = -50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxPitch = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 180.0f))
	float ZiplineCameraMinYaw = -30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 180.0f))
	float ZiplinerCameraMaxYaw = 30.0f;


	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;

	virtual void OnHardLanding() override;

private:

	void OnLadderStarted();
	void OnLadderStopped();

	void OnZiplineStarted();
	void OnZiplineStopped();

	FTimerHandle FPMontageTimer; // таймер для возвращения возможности движения и поворота после подтягивания

	bool IsFPMontagePlaying() const;

	void OnFPMontageTimerElapsed(); // метод, который будет срабатывать по окончании таймера

	TWeakObjectPtr<class ASMPlayerController> SMPlayerController;

};
