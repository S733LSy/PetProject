
#include "WidgetCharacterAttributes.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

ASMBaseCharacter* UWidgetCharacterAttributes::GetCharacterOwner() const
{
	APawn* Pawn = GetOwningPlayerPawn();
	ASMBaseCharacter* Character = Cast<ASMBaseCharacter>(Pawn);
	return Character;
}

UCharacterAttributeComponent* UWidgetCharacterAttributes::GetOwningCharacterAttributeComponent() const
{
	return GetCharacterOwner()->GetCharacterAttributeComponent();
}

float UWidgetCharacterAttributes::GetOxygenPercent() const
{
	float Result = 1.0f;
	if (IsValid(GetCharacterOwner()))
	{
		Result = GetOwningCharacterAttributeComponent()->GetOxygenPercent();
	}
	return Result;
}

float UWidgetCharacterAttributes::GetStaminaPercent() const
{
	float Result = 1.0f;
	if (IsValid(GetCharacterOwner()))
	{
		Result = GetOwningCharacterAttributeComponent()->GetStaminaPercent();
	}
	return Result;
}

float UWidgetCharacterAttributes::GetHealthPercent() const
{
	float Result = 1.0f;
	if (IsValid(GetCharacterOwner()))
	{
		Result = GetOwningCharacterAttributeComponent()->GetHealthPercent();
	}
	return Result;
}