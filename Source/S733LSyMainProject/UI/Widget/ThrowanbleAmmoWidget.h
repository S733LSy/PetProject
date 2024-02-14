// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThrowanbleAmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API UThrowableAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 Ammo;

private:
	UFUNCTION()
	void UpdateThrowableAmmo(int32 NewAmmo);

	TWeakObjectPtr<class ASMBaseCharacter> CachedBaseCharacter;
};
