// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAniminstance.h"
#include "../PlayerCharacter.h"

void UBaseCharacterAniminstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<APlayerCharacter>(), TEXT("UBaseCharacterAniminstance::NativeBeginPlay() can be used with character only"));
	ChachedPlayerCharacterOwner = StaticCast<APlayerCharacter*>(TryGetPawnOwner());

}

void UBaseCharacterAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ChachedPlayerCharacterOwner.IsValid())
	{
		return;
	}
	RightFootEffectorLocation = FVector(0.0f, ChachedPlayerCharacterOwner->GetIKRightFootOffset(), 0.0f);
	LeftFootEffectorLocation = FVector(0.0f, ChachedPlayerCharacterOwner->GetIKLeftFootOffset(), 0.0f);
}
