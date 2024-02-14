// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Intaractive/Interface/Interactive.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Inventory/Items/InventoryItem.h"
#include "SubSystems/SaveSubSystem/SaveSubsystemInterface.h"
#include "Door.generated.h"

UCLASS()
class S733LSYMAINPROJECT_API ADoor : public AActor, public IInteractable, public ISaveSubsystemInterface
{
	GENERATED_BODY()
	
public:	
	ADoor();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact(ASMBaseCharacter* Character) override;

	virtual FName GetActionEventName() const override;

	virtual bool HasOnInteractionCallback() const override;
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override;
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override;

	//@ ISaveSubsystemInterface
	virtual void OnLevelDeserialized_Implementation() override; // некий BeginPlay
	//~ISaveSubsystemInterface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh; // мешка двери

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	USceneComponent* DoorPivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;

	virtual void BeginPlay() override;

	IInteractable::FOnInteraction OnInteractionEvent;

private:
	void InteractWithDoor();

	UFUNCTION()
	void UpdateDoorAnimation(float Alpha);// вызывается в момент тика таймлайна

	UFUNCTION()
	void OnDoorAnimationFinished();

	FTimeline DoorOpenTimeline;

	UPROPERTY(SaveGame) // именно данный атрибут отвечает за то, будет это свойство сериализованно или десериализованно
	bool bIsOpened = false;
};
