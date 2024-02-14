// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactive.generated.h"

class ASMBaseCharacter;
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class S733LSYMAINPROJECT_API IInteractable
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE(FOnInteraction);

	virtual void Interact(ASMBaseCharacter* Character) PURE_VIRTUAL(IInteractable::Interact, );

	virtual FName GetActionEventName() const PURE_VIRTUAL(IInteractable::GetActionEventName, return FName(NAME_None););

	virtual bool HasOnInteractionCallback() const PURE_VIRTUAL(IInteractable::HasOnInteractionCallback, return false;); // выбор подходящего эктора на сцене
	virtual FDelegateHandle AddOnInteractionUFunction(UObject * Object, const FName & FunctionName) PURE_VIRTUAL(IInteractable::AddOnInteractionDelegate, return FDelegateHandle(););
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) PURE_VIRTUAL(IInteractable::RemoveOnInteractionDelegate, );
};
