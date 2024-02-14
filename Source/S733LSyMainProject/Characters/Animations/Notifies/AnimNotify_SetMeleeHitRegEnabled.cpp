
#include "AnimNotify_SetMeleeHitRegEnabled.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ASMBaseCharacter* CharacterOwner = Cast<ASMBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	AMeleeWeaponItem* MeleeWeapon =  CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(MeleeWeapon))
	{
		MeleeWeapon->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
