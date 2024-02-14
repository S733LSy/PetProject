// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


AMPBaseCharacter::AMPBaseCharacter(const FObjectInitializer& ObjectInitializer)
{

}


void AMPBaseCharacter::ChangeCrouchState()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

