// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Intaractive/Interface/Interactive.h"
#include "GameFramework/Actor.h"
#include "PickableItem.generated.h"

UCLASS(Abstract, NotBlueprintable)
class S733LSYMAINPROJECT_API APickableItem : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	const FName& GetDataTableID() const;
	void SetCount(int32 Count_In);
	int32 GetCount() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName DataTableID = NAME_None;

private:
	int32 Count;
};
