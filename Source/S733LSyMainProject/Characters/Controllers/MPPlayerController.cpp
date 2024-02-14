// Fill out your copyright notice in the Description page of Project Settings.


#include "MPPlayerController.h"
#include "Characters/MPBaseCharacter.h"

void AMPPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AMPBaseCharacter>(InPawn);
}

void AMPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AMPPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMPPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AMPPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AMPPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AMPPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AMPPlayerController::LookUpAtRate);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMPPlayerController::Jump);//&AMPPlayerController::Jump - адрес метода
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AMPPlayerController::ChangeCrouchState);
}

void AMPPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AMPPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AMPPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AMPPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AMPPlayerController::TurnAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}

void AMPPlayerController::LookUpAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}

void AMPPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AMPPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}
