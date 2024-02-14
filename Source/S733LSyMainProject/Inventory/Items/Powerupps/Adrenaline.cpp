
#include "Adrenaline.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Characters/SMBaseCharacter.h"

bool UAdrenaline::Consume(ASMBaseCharacter* ConsumeTarget)
{
	UCharacterAttributeComponent* CharacterAttributes = ConsumeTarget->GetCharacterAttributeComponent();
	CharacterAttributes->RestoreFullStamina();
	this->ConditionalBeginDestroy();
	return true;
}
