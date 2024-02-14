// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetMeleeHitRegEnabled.generated.h"


UCLASS()
class S733LSYMAINPROJECT_API UAnimNotify_SetMeleeHitRegEnabled : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee weapon") // Для нотификаторов лучше ставить EditAnywhere
	bool bIsHitRegistrationEnabled = false;
};
