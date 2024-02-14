// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S733LSyMainProjectBasePawn.h"
#include "SpiderPawn.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API ASpiderPawn : public AS733LSyMainProjectBasePawn
{
	GENERATED_BODY()

public:
	ASpiderPawn();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFrontFootOffset() const {return IKRightFrontFootOffset;}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightRearFootOffset() const { return IKRightRearFootOffset; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFrontFootOffset() const { return IKLeftFrontFootOffset; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftRearFootOffset() const { return IKLeftRearFootOffset; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spider bot")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName RightFrontFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName RightRearFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName LeftFrontFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName LeftRearFootSocketName;

private:
	float IKRightFrontFootOffset = 0.0f;
	float IKRightRearFootOffset = 0.0f;
	float IKLeftFrontFootOffset = 0.0f;
	float IKLeftRearFootOffset = 0.0f;
	
};
