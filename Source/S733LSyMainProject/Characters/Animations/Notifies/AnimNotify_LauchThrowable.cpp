
#include "AnimNotify_LauchThrowable.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_LauchThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ASMBaseCharacter* CharacterOwner = Cast<ASMBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	if (CharacterOwner->GetLocalRole() < ROLE_Authority)
	{
		return;;
	}
	
	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->LaunchCurrentThrowableItem();
}
