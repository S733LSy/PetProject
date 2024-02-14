// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSMPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/SMPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S733LSyMainProject/S733LSyMainProjectTypes.h"
#include "Components/MovementComponents/SMBaseCharacterMovementComponent.h"

AFPSMPlayerCharacter::AFPSMPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true); // ����� �� ������ �����, ���� ��������� �� 3 ����

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true); // ����� �� ������ ����� �� 3 ����, ������� ���� ����
	GetMesh()->bCastHiddenShadow = true; // ������� ���� �� ����� �� ������� ����

	CameraComponent->bAutoActivate = false; // ����� ������-�������� ������ ������ ��� ������� ����, � �� 3��
	SpringArmComponent->bAutoActivate = false; // ����� ������� ����� ������ ����
	SpringArmComponent->bUsePawnControlRotation = false; // � ������ ���� ������� ����������

	GetCharacterMovement()->bOrientRotationToMovement = false; // ������� ������� �� ��������� ���������, �.�. �� ������� ���� ����� ������������ ��������� ������, � �� ��� ���� �����
	bUseControllerRotationYaw = true; // �������, ������� ����� ��� ��������� �� ������� ����
}

void AFPSMPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); // ���������� ������ �� �������
	SMPlayerController = Cast<ASMPlayerController>(NewController);
}

void AFPSMPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsFPMontagePlaying() && SMPlayerController.IsValid())
	{
		FRotator TargetControlRotation = SMPlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0.0f; // ����� ����� ������� ����������
		float BlendSpeed = 300.0f;
		TargetControlRotation = FMath::RInterpTo(SMPlayerController->GetControlRotation(), TargetControlRotation, DeltaTime, BlendSpeed);
		SMPlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPSMPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPSMPlayerCharacter* DefaultCharacter = GetDefault<AFPSMPlayerCharacter>(GetClass()); // GetClass() ���������� ������������ ����� � ������������ ��������� �� ���������, ��� ���� �� �����
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable(); // ���������� �� ����������� ������, ������� ����� ��������������
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPSMPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPSMPlayerCharacter* DefaultCharacter = GetDefault<AFPSMPlayerCharacter>(GetClass()); // GetClass() ���������� ������������ ����� � ������������ ��������� �� ���������, ��� ���� �� �����
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable(); // ���������� �� ����������� ������, ������� ����� ��������������
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

FRotator AFPSMPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();

	if (IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}

	return Result;
}

void AFPSMPlayerCharacter::OnLadderStarted()
{
	if (SMPlayerController.IsValid())
	{
		SMPlayerController->SetIgnoreCameraPitch(true); // ����� ������ � ������ ����������, ����� get ��������� � �������� ������ UFPPlayerrAnimInstance::CalculateCameraPitchAngle()
		bUseControllerRotationYaw = false; // ���������� �������� ��������� �����/����, �� ���������� ����� ��� ���� �� �����
		APlayerCameraManager* CameraManager = SMPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = LadderCameraMinPitch;
		CameraManager->ViewPitchMax = LadderCameraMaxPitch;
		CameraManager->ViewYawMin = GetActorForwardVector().ToOrientationRotator().Yaw + LadderCameraMinYaw;
		CameraManager->ViewYawMax = GetActorForwardVector().ToOrientationRotator().Yaw + LadderCameraMaxYaw;
	}
}

void AFPSMPlayerCharacter::OnLadderStopped()
{
	if (SMPlayerController.IsValid())
	{
		SMPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = SMPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

void AFPSMPlayerCharacter::OnZiplineStarted()
{
	if (SMPlayerController.IsValid())
	{
		SMPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		APlayerCameraManager* CameraManager = SMPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = ZiplineCameraMinPitch;
		CameraManager->ViewPitchMax = ZiplineCameraMaxPitch;
		CameraManager->ViewYawMin = GetActorForwardVector().ToOrientationRotator().Yaw + ZiplineCameraMinYaw;
		CameraManager->ViewYawMax = GetActorForwardVector().ToOrientationRotator().Yaw + ZiplinerCameraMaxYaw;
	}
}

void AFPSMPlayerCharacter::OnZiplineStopped()
{
	if (SMPlayerController.IsValid())
	{
		SMPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = SMPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

void AFPSMPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		OnLadderStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		OnLadderStopped();
	}
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		OnZiplineStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		OnZiplineStopped();
	}
}

void AFPSMPlayerCharacter::OnHardLanding()
{
	Super::OnHardLanding();
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && FPBaseCharacterHardLandingAnimMontage)
	{
		if (SMPlayerController.IsValid())
		{
			SMPlayerController->SetIgnoreLookInput(true); // ������������� ����� �� ��������
			SMPlayerController->SetIgnoreMoveInput(true); // ������������� ����� �� ������������
		}
		float MontageDuration = FPAnimInstance->Montage_Play(FPBaseCharacterHardLandingAnimMontage, 1.0f, EMontagePlayReturnType::Duration);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPSMPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPSMPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantlingSettings.FPMantlingMontage)
	{
		if (SMPlayerController.IsValid())
		{
			SMPlayerController->SetIgnoreLookInput(true); // ������������� ����� �� ��������
			SMPlayerController->SetIgnoreMoveInput(true); // ������������� ����� �� ������������
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime); // �������� ������������ �������, �� � ��� ������� ��� � ������������ �������
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPSMPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPSMPlayerCharacter::OnFPMontageTimerElapsed()
{
	if (SMPlayerController.IsValid())
	{
		SMPlayerController->SetIgnoreLookInput(false); // ������������� ����� �� ��������
		SMPlayerController->SetIgnoreMoveInput(false); // ������������� ����� �� ������������
	}
}

bool AFPSMPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}
