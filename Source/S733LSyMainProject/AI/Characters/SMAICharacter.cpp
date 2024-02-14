
#include "SMAICharacter.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"

ASMAICharacter::ASMAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIPatrolingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrolling"));
}

UAIPatrollingComponent* ASMAICharacter::GetPatrollingComponent() const
{
	return AIPatrolingComponent;
}

UBehaviorTree* ASMAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}
