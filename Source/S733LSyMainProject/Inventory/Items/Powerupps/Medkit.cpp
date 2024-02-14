
#include "Medkit.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Characters/SMBaseCharacter.h"

bool UMedkit::Consume(ASMBaseCharacter* ConsumeTarget)
{
	UCharacterAttributeComponent* CharacterAttributes = ConsumeTarget->GetCharacterAttributeComponent();
	CharacterAttributes->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true;
}
