// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocator.h"

// Sets default values
APlatformInvocator::APlatformInvocator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APlatformInvocator::Invoke()
{
	//TODO
}

// Called when the game starts or when spawned
void APlatformInvocator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlatformInvocator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

