// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/TimelineTemplate.h"
#include "PlatformInvocator.generated.h"


UCLASS()
class S733LSYMAINPROJECT_API APlatformInvocator : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APlatformInvocator();

	UFUNCTION(BlueprintCallable)
		void Invoke();
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
