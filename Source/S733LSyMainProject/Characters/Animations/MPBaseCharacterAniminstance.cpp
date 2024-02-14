// Fill out your copyright notice in the Description page of Project Settings.


#include "MPBaseCharacterAniminstance.h"
#include "../MPBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMPBaseCharacterAniminstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AMPBaseCharacter>(), TEXT("UMPBaseCharacterAniminstance::NativeBeginPlay() can be used only witn AMPBaseCharacter"));
	ChachedBaseCharacter = StaticCast<AMPBaseCharacter*>(TryGetPawnOwner());
}

void UMPBaseCharacterAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!ChachedBaseCharacter.IsValid())
	{
		return;
	}

	UCharacterMovementComponent* CharacterMovement = ChachedBaseCharacter->GetCharacterMovement();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();

}
