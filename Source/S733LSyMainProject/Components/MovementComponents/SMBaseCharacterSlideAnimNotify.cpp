// Fill out your copyright notice in the Description page of Project Settings.


#include "SMBaseCharacterSlideAnimNotify.h"

#include "SMBaseCharacterMovementComponent.h"
#include "Characters/SMBaseCharacter.h"

void USMBaseCharacterSlideAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ASMBaseCharacter* CharacterOwner = Cast<ASMBaseCharacter>(MeshComp->GetOwner());
	if (IsValid(CharacterOwner))
	{
		CharacterOwner->GetBaseCharacterMovementComponent()->EndSlide();
	}
}
