
#pragma once

#include "CoreMinimal.h"
#include "Characters/SMBaseCharacter.h"
#include "SMAICharacter.generated.h"

class UAIPatrollingComponent;
class UBehaviorTree;
UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API ASMAICharacter : public ASMBaseCharacter
{
	GENERATED_BODY()

public:
	ASMAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrollingComponent* GetPatrollingComponent() const;

	UBehaviorTree* GetBehaviorTree() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
	UAIPatrollingComponent* AIPatrolingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
