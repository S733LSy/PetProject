// Fill out your copyright notice in the Description page of Project Settings.


#include "S733LSyMainProjectBasePawn.h"
#include <Components/SphereComponent.h>
#include <Engine/CollisionProfile.h>
#include <GameFramework/FloatingPawnMovement.h>
#include "../Components/MovementComponents/S733LSyPawnMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Camera/PlayerCameraManager.h>
#include "../S733LSyMainProject.h"

// Sets default values
AS733LSyMainProjectBasePawn::AS733LSyMainProjectBasePawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    CollisionComponent->SetSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    RootComponent = CollisionComponent;

    //MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement Component"));
    MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, US733LSyPawnMovementComponent>(TEXT("Movement Component"));
    MovementComponent->SetUpdatedComponent(CollisionComponent);

}

// Called to bind functionality to input
void AS733LSyMainProjectBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("MoveForward", this, &AS733LSyMainProjectBasePawn::MoveForward);// ѕрив€зка методов к ос€м
    PlayerInputComponent->BindAxis("MoveRight", this, &AS733LSyMainProjectBasePawn::MoveRight);
    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AS733LSyMainProjectBasePawn::Jump);
}

void AS733LSyMainProjectBasePawn::MoveForward(float Value)
{
    InputForward = Value;
    if (Value != 0.0f)
    {
        AddMovementInput(CurrentViewActor->GetActorForwardVector(), Value); // «адаетс€ направление, в котром будет двигатьс€ персонаж
    }
}

void AS733LSyMainProjectBasePawn::MoveRight(float Value)
{
    InputRight = Value;
    if (Value != 0.0f)
    {
        AddMovementInput(CurrentViewActor->GetActorRightVector(), Value);
    }
}

void AS733LSyMainProjectBasePawn::Jump()
{
    checkf(MovementComponent->IsA<US733LSyPawnMovementComponent>(), TEXT("AS733LSyMainProjectBasePawn::Jump() Jump can only work with US733LSyPawnMovementComponent"))
    US733LSyPawnMovementComponent* BaseMovement = StaticCast<US733LSyPawnMovementComponent*>(MovementComponent);
    BaseMovement->JumpStart();
}

void AS733LSyMainProjectBasePawn::BeginPlay()
{
    Super::BeginPlay();
    APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    CurrentViewActor = CameraManager->GetViewTarget();
    CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));

}

void AS733LSyMainProjectBasePawn::OnBlendComplete()
{
    CurrentViewActor = GetController()->GetViewTarget();
    UE_LOG(LogCameras, Log, TEXT("AS733LSyMainProjectBasePawn::OnBlendComplete() current view actor: %s"), *CurrentViewActor->GetName());
}

