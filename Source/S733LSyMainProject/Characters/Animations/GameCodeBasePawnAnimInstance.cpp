// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCodeBasePawnAnimInstance.h"
#include "../S733LSyMainProjectBasePawn.h"
#include "Components/MovementComponents/S733LSyPawnMovementComponent.h"

void UGameCodeBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AS733LSyMainProjectBasePawn>(), TEXT("UGameCodeBasePawnAnimInstance::NativeBeginPlay() only S733LSyMainProjectBasePawn can work with UGameCodeBasePawnAnimInstance"))
	CachedBasePawn = StaticCast<AS733LSyMainProjectBasePawn*>(TryGetPawnOwner());
}

void UGameCodeBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBasePawn.IsValid())
	{
		return;
	}

	InputForward = CachedBasePawn->GetInputForward();
	InputRight = CachedBasePawn->GetInputRight();
	bIsInAir = CachedBasePawn->GetMovementComponent()->IsFalling();
}