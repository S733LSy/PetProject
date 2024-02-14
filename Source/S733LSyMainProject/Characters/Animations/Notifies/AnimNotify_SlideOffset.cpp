// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SlideOffset.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/MovementComponents/SMBaseCharacterMovementComponent.h"

void UAnimNotify_SlideOffset::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ASMBaseCharacter* CharacterOwner = Cast<ASMBaseCharacter>(MeshComp->GetOwner());
	if (IsValid(CharacterOwner))
	{
		CharacterOwner->GetBaseCharacterMovementComponent()->StartSlideMeshOffset();
	}
}
