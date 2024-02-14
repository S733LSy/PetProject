// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveSubsystemUtils.generated.h"

class ULevel;
UCLASS()
class S733LSYMAINPROJECT_API USaveSubsystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Save Subsystem Utils")
	static void BroadcastOnLevelDeserialized(ULevel* Level);
	// внутри обходит всех экторов данного уровня, определять какой из них реализовывает сабсистем интерфейс и если да, то будет вызывать метод OnLevelDeserialized
};
