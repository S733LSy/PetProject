// Fill out your copyright notice in the Description page of Project Settings.


#include "SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../Components/MovementComponents/SMBaseCharacterMovementComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Controllers/SMPlayerController.h"
#include "Kismet/GameplayStatics.h"

ASMPlayerCharacter::ASMPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer) // явное объявление
// настройка персонажа
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent); // прикрепление к root компоненту
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent); // прикрепление к springarmcomponent
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	Team = ETeams::Player;
}

void ASMPlayerCharacter::MoveForward(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f); // для поворота персонажа за поворотом камеры
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector); // для поворота персонажа за поворотом камеры
		//AddMovementInput(GetActorForwardVector(), Value);
		AddMovementInput(ForwardVector, Value);
	}
}

void ASMPlayerCharacter::MoveRight(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f); // для поворота персонажа за поворотом камеры
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector); // для поворота персонажа за поворотом камеры
		//AddMovementInput(GetActorRightVector(), Value);
		AddMovementInput(RightVector, Value);
	}
}	

void ASMPlayerCharacter::Turn(float Value)
{
	if (IsAiming())
	{
		AddControllerYawInput(Value * GetCurrentRangeWeaponItem()->GetAimTurnModifier());
	}
	else
	{
	AddControllerYawInput(Value);
	}
}

void ASMPlayerCharacter::LookUp(float Value)
{
	if (IsAiming())
	{
		AddControllerPitchInput(Value * GetCurrentRangeWeaponItem()->GetAimLookUpModifier());
	}
	else
	{
		AddControllerPitchInput(Value);
	}
}

void ASMPlayerCharacter::TurnAtRate(float Value)
{
	if (IsAiming())
	{
		AddControllerYawInput(Value * GetCurrentRangeWeaponItem()->GetAimTurnModifier() * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ASMPlayerCharacter::LookUpAtRate(float Value)
{
	if (IsAiming())
	{
		AddControllerPitchInput(Value * GetCurrentRangeWeaponItem()->GetAimLookUpModifier() * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ASMPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaleHalfHeightAdjust); // вызывается метод базового класса, если это проигнорировать мешка персонажа не будет перестраивать свою позицию относительно капсулы
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust); // меняем таргет оффсет, прибавляя к его высоте HalfHeightAdjust
}

void ASMPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaleHalfHeightAdjust); // вызывается метод базового класса, если это проигнорировать мешка персонажа не будет перестраивать свою позицию относительно капсулы
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust); // меняем таргет оффсет, вычитая к его высоте HalfHeightAdjust
}

void ASMPlayerCharacter::OnStartProne()
{
	{
		if (ASMPlayerCharacter::GetMesh())
		{
			FVector& MeshRelativeLocation = ASMPlayerCharacter::GetMesh()->GetRelativeLocation_DirectMutable();
			MeshRelativeLocation.Z = ASMPlayerCharacter::GetMesh()->GetRelativeLocation().Z + HeightAdjust;
			BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		}
	}
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, 20.0f);
	ASMBaseCharacter::OnStartProne();
}

void ASMPlayerCharacter::OnEndProne()
{
	if (ASMPlayerCharacter::GetMesh())
	{
		FVector& MeshRelativeLocation = ASMPlayerCharacter::GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = ASMPlayerCharacter::GetMesh()->GetRelativeLocation().Z - HeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, 20.0f);
}

void ASMPlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f); // для поворота персонажа за поворотом камеры
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector); // для поворота персонажа за поворотом камеры
		//AddMovementInput(GetActorForwardVector(), Value);
		AddMovementInput(ForwardVector, Value);
	}
}

void ASMPlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f); // для поворота персонажа за поворотом камеры
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector); // для поворота персонажа за поворотом камеры
		AddMovementInput(RightVector, Value);
	}
}

void ASMPlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

bool ASMPlayerCharacter::CanJumpInternal_Implementation() const
{
	if (!SMBaseCharacterMovementComponent->IsMantling())
	{
		return bIsCrouched || Super::CanJumpInternal_Implementation();
	}
	else
	{
		return false;
	}
}

void ASMPlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
		SetbIsProneRequested();
	}
}

void ASMPlayerCharacter::Tick(float DeltaTime)
{
	ASMBaseCharacter::Tick(DeltaTime);
	TryChangeSprintState(DeltaTime);
	SpringArmTimeline.TickTimeline(DeltaTime);
	AimFOVTimeline.TickTimeline(DeltaTime);
	if (SMBaseCharacterMovementComponent->IsSprinting())
	{
		BeginSpringArmTimeline();
	}
	else
	{
		EndSpringArmTimeline();
	}
}

void ASMPlayerCharacter::BeginPlay()
{
	ASMBaseCharacter::BeginPlay();
	SpringArmComponent->TargetArmLength = DefaultSpringArmLength;
	if (IsValid(SpringArmCurveFloat))
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateSpringArmTimeline"));
		SpringArmTimeline.AddInterpFloat(SpringArmCurveFloat, TimelineCallback);
	}
	if (IsValid(AimCurveFloat))
	{
		FOnTimelineFloatStatic AimTimelineCallback;
		AimTimelineCallback.BindUObject(this, &ASMPlayerCharacter::UpdateAimFOVTimeline);
		AimFOVTimeline.AddInterpFloat(AimCurveFloat,AimTimelineCallback);
		AimFOVTimeline.SetLooping(false);
	}
}

void ASMPlayerCharacter::UpdateSpringArmTimeline(float Alpha)
{
	const float SpringArmTargetLocation = FMath::Lerp(DefaultSpringArmLength, SprintSpringArmLength, Alpha);
	SpringArmComponent->TargetArmLength = SpringArmTargetLocation;
}

void ASMPlayerCharacter::UpdateAimFOVTimeline(float Alpha)
{
	const float TargetAimFOV = FMath::Lerp(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->DefaultFOV, CharacterEquipmentComponent->GetCurrentRangeWeapon()->GetAimFOV(), Alpha);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->SetFOV(TargetAimFOV);
}

void ASMPlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	if(ensure(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
	{
		if (AimCurveFloat)
		{
			AimFOVTimeline.Play();
		}
		else
		{
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->SetFOV(GetCurrentRangeWeaponItem()->GetAimFOV());
		}
	}
}

void ASMPlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	if(ensure(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
	{
		if (AimCurveFloat)
		{
			AimFOVTimeline.Reverse();
		}
		else
		{
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->UnlockFOV();
		}
	}
}

ARangeWeaponItem* ASMPlayerCharacter::GetCurrentRangeWeaponItem() const
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	return IsValid(CurrentRangeWeapon) ? CurrentRangeWeapon : nullptr;
}
