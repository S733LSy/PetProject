
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SMAIController.generated.h"

class UAISense;
UCLASS()
class S733LSYMAINPROJECT_API ASMAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASMAIController();

protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const;
	
};
