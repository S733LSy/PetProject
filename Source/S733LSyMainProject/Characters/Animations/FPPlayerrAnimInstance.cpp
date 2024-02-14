// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerrAnimInstance.h"
#include "Characters/FPSMPlayerCharacter.h"
#include "Characters/Controllers/SMPlayerController.h"

void UFPPlayerrAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPSMPlayerCharacter>(), TEXT("UFPPlayerrAnimInstance::NativeBeginPlay() only AFPSMPlayerCharacter can use UFPPlayerrAnimInstance"));
	CachedFirstPersonCharacterOwner = StaticCast<AFPSMPlayerCharacter*>(TryGetPawnOwner());
}

void UFPPlayerrAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedFirstPersonCharacterOwner.IsValid())
	{
		return;
	}
	PlayerCameraPitchAngle = CalculateCameraPitchAngle();

}

float UFPPlayerrAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.0f;
	ASMPlayerController* Controller = CachedFirstPersonCharacterOwner->GetController<ASMPlayerController>();
	if (IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
	{
		Result = Controller->GetControlRotation().Pitch;
	}
	return Result;
}
