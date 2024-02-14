
#include "SMBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "S733LSyMainProject/Actors/Intaractive/Environment/Ladder.h"
#include "S733LSyMainProject/Components/LedgeDetectorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "S733LSyMainProject/Actors/Intaractive/Environment/ZiplineInteractiveActor.h"

FNetworkPredictionData_Client* USMBaseCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		USMBaseCharacterMovementComponent* MutableThis = const_cast<USMBaseCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Character_SM(*this);
	}
	return ClientPredictionData;
}

void USMBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	/*
FLAG_Reserved_1 = 0x04,	// Reserved for future use
FLAG_Reserved_2 = 0x08,	// Reserved for future use
// Remaining bit masks are available for custom flags.
FLAG_Custom_0 = 0x10, - Sprinting flag
FLAG_Custom_1 = 0x20, - Mantling flag
FLAG_Custom_2 = 0x40,
FLAG_Custom_3 = 0x80,
*/

	bool bWasMantling = GetBaseCharacterOwner()->bIsMantling;
	bIsSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0; // &= побитовое или
	bool bIsMantling = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (!bWasMantling && bIsMantling)
		{
			GetBaseCharacterOwner()->Mantle(true);
		}
	}
}

void USMBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		const float AngleTolerance = 1e-3f;
		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}

	if (IsOnLadder()) // если персонаж на лестнице, отключается вращение
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

float USMBaseCharacterMovementComponent::GetMaxSpeed() const
{
    float Result = Super::GetMaxSpeed(); // запись в переменную значение возвращаемое базовым методом
    if (bIsSprinting)
    {
        Result = SprintSpeed;
    }
    if (IsOutOfStamina())
    {
        Result = OutOfStaminaSpeed;
    }
    if (bIsProne)
    {
        Result = MaxProneSpeed;
    }
	if (IsOnLadder())
	{
		Result = ClimbingOnLadderMaxSpeed;
	}
	if (IsOnZipline())
	{
		Result = MaxZiplineSpeed;
	}
	if (GetBaseCharacterOwner()->IsAiming())
	{
		Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}
	if (bIsSliding)
	{
		Result = 1500.0f;
	}
    return Result;
}

void USMBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_Im)
{
	bIsOutOfStamina = bIsOutOfStamina_Im;
	if (bIsOutOfStamina)
	{
		StopSprint();
	}
}

void USMBaseCharacterMovementComponent::StartSprint()
{
    if (Velocity.Size() > 0.0f)
    {
    bIsSprinting = true;
    bForceMaxAccel = 1;
    }
}

void USMBaseCharacterMovementComponent::StopSprint()
{
    bIsSprinting = false;
    bForceMaxAccel = 0;
}

bool USMBaseCharacterMovementComponent::bEnProne() const
{
	FCollisionResponseParams ResponseParam;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CheckLedgeDirection), false, CharacterOwner);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	const UWorld* MyWorld = GetWorld();
	const float OldUnscaledHalfHeight = 60.0f;
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, 34.0f, ProneHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
	bool bEnProne = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
	return bEnProne;
}

void USMBaseCharacterMovementComponent::Prone()
{
	bIsProne = true;
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, ProneHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void USMBaseCharacterMovementComponent::UnProne()
{
	const float OldUnscaledHalfHeight = 60.0f;
	bIsProne = false;
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, 34.0f, ProneHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(34.0f, OldUnscaledHalfHeight);
	UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void USMBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
    CurrentMantlingParameters = MantlingParameters;
    SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void USMBaseCharacterMovementComponent::EndMantle()
{
	GetBaseCharacterOwner()->bIsMantling = false;
    SetMovementMode(MOVE_Walking);
}

bool USMBaseCharacterMovementComponent::IsMantling() const
{
    return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

float USMBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("USMBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection(FVector & LadderUpVector, float& ActorToLadderProjection) cannot be invoked when current ladder is null"));

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation(); // расстояние от основания лестницы до персонажа
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
	// второй направлен в направлении вверх и идёт по лестнице, получается что надо найти один из катетов, находится он с помощью проекции
}

void USMBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
    CurrentLadder = Ladder;

	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator(); // Получаем угол поворота
	TargetOrientationRotation.Yaw += 180.0f; // прибавляем 180 градусов, так как лестница смотрит прямо

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());

    FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector; // получаем новые координаты для персонажа
	if (CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}

	GetOwner()->SetActorLocation(NewCharacterLocation); // устанавливаем новую позицию персонажа
	GetOwner()->SetActorRotation(TargetOrientationRotation); // поворачиваем персонажа лицом к лестнице

    SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

void USMBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
		case EDetachFromLadderMethod::JumpOff:
			{
				FVector JumpDirection = CurrentLadder->GetActorForwardVector();
				SetMovementMode(MOVE_Falling);

				FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
				JumpVelocity.Z += JumpUpOffLadderHeight;
				ForceTargetRotation = JumpDirection.ToOrientationRotator();
				bForceRotation = true;
				Launch(JumpVelocity);
				break;
			}
		case EDetachFromLadderMethod::ReachingTheTop:
			{
				GetBaseCharacterOwner()->Mantle(true);
				break;
			}
		case EDetachFromLadderMethod::ReachingTheBottom:
			{
				SetMovementMode(MOVE_Walking);
				break;
			}
		case EDetachFromLadderMethod::Fall:
		default:
			{
				SetMovementMode(MOVE_Falling);
				break;
			}
	}
}

bool USMBaseCharacterMovementComponent::IsOnLadder() const
{
    return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

const ALadder* USMBaseCharacterMovementComponent::GetCurrentLadder()
{
	return CurrentLadder;
}

void USMBaseCharacterMovementComponent::AttachToZipline(const AZiplineInteractiveActor* Zipline)
{
	CurrentZipline = Zipline;
	FRotator ZiplineInteractiveCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(CurrentZipline->GetTopPointOfEndPillar(), CurrentZipline->GetTopPointOfStartPillar());
	FRotator TargetOrientationRotation = FRotator(0.0f, ZiplineInteractiveCapsuleRotation.Yaw, ZiplineInteractiveCapsuleRotation.Roll);
	float Projection = GetActorToCurrentZiplineProjection(GetActorLocation());
	FVector ZiplineForwardVector = CurrentZipline->GetZiplineInteractionCapsule()->GetForwardVector();
	FVector ZiplineUpVector = CurrentZipline->GetZiplineInteractionCapsule()->GetUpVector();

	FVector NewCharacterLocation = (CurrentZipline->GetZiplineInteractionCapsule()->GetComponentLocation() - CurrentZipline->GetZiplineAnimationOffset()) + Projection * ZiplineUpVector - (CurrentZipline->GetZiplineInteractionCapsule()->GetUnscaledCapsuleRadius() - ZiplineToCharacterOffset) * ZiplineForwardVector;

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void USMBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

float USMBaseCharacterMovementComponent::GetActorToCurrentZiplineProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentZipline), TEXT("USMBaseCharacterMovementComponent::GetActorToCurrentZiplineProjection cannot be invoked when current zipline is null"))
	FVector ZiplineUpVector = CurrentZipline->GetZiplineInteractionCapsule()->GetUpVector();
	FVector ZiplineToCharacterDistance = Location - CurrentZipline->GetZiplineInteractionCapsule()->GetComponentLocation();
	return FVector::DotProduct(ZiplineUpVector, ZiplineToCharacterDistance);
}

bool USMBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

const AZiplineInteractiveActor* USMBaseCharacterMovementComponent::GetCurrentZipline()
{
	return CurrentZipline;
}

float USMBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("USMBaseCharacterMovementComponent::GetLadderSpeedRatio() cannot be invoked when current ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector(); // вектор оси лестницы
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;
}

void USMBaseCharacterMovementComponent::StartSlideMeshOffset()
{
	const float MeshRelativeLocationX = CharacterOwner->GetMesh()->GetRelativeLocation().X;
	const float MeshRelativeLocationY = CharacterOwner->GetMesh()->GetRelativeLocation().Y;
	const float MeshRelativeLocationZ = CharacterOwner->GetMesh()->GetRelativeLocation().Z;
	CharacterOwner->GetMesh()->SetRelativeLocation(FVector(MeshRelativeLocationX, MeshRelativeLocationY, MeshRelativeLocationZ + SlideOffset));
}

void USMBaseCharacterMovementComponent::StartSlide()
{
	if (bIsSliding)
	{
		return;
	}

	bIsSliding = true;

	UAnimInstance* CharacterAnimInstance = GetBaseCharacterOwner()->GetMesh()->GetAnimInstance();
	if (IsValid(CharacterAnimInstance) && IsValid(SlideAnimMontage))
	{
		const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, SlideCapsuleHalfHeight);
		CharacterAnimInstance->Montage_Play(SlideAnimMontage, 1.0f, EMontagePlayReturnType::Duration);
	}
}

void USMBaseCharacterMovementComponent::EndSlide()
{
	bIsSliding = false;

	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, 90.0f);
	const float MeshRelativeLocationX = CharacterOwner->GetMesh()->GetRelativeLocation().X;
	const float MeshRelativeLocationY = CharacterOwner->GetMesh()->GetRelativeLocation().Y;
	const float MeshRelativeLocationZ = CharacterOwner->GetMesh()->GetRelativeLocation().Z;
	CharacterOwner->GetMesh()->SetRelativeLocation(FVector(MeshRelativeLocationX, MeshRelativeLocationY, MeshRelativeLocationZ - SlideOffset));
}

void USMBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;
	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation() + CurrentMantlingParameters.TargetActor->GetComponentLocation();
	Velocity = Delta / DeltaTime;
	FHitResult Hit;

	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit); // метод для перемещения компонента
}

void USMBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations) // Надо найти дельту, смещение которое будет происходить в этом кадре, на котором мы будет двигать наш эктор
{
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBrakingDeceleration); // BrakingDeceleration - параметр, который выставляет значение уменьшенияя ускорения, как только отпускается инпут, чтобы персонаж резко не замедлялся или ускорялся и наоборот
	FVector Delta = Velocity * DeltaTime; // смещение которое будет происходить в этом кадре, на котором мы будет двигать наш эктор

	if (HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit); // метод для перемещения компонента
		return;
	}

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if ((NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset)) && CurrentLadder->GetIsOnTop() == false)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit); // метод для перемещения компонента
}

void USMBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations)
{
	FVector ZiplineDirection = (CurrentZipline->GetTopPointOfStartPillar() - CurrentZipline->GetTopPointOfEndPillar()).GetSafeNormal();
	FVector Delta = MaxZiplineSpeed * ZiplineDirection * DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	if (Hit.IsValidBlockingHit())
	{
		SetMovementMode(MOVE_Walking);
	}
}

bool USMBaseCharacterMovementComponent::GetIsSliding()
{
	return bIsSliding;
}

void USMBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		return;
	}

	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(DeltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_Ladder:
		{
			PhysLadder(DeltaTime, Iterations);
	        break;
		}
	case (uint8)ECustomMovementMode::CMOVE_Zipline:
		{
			PhysZipline(DeltaTime, Iterations);
			break;
		}
		default:
            break;
	}
	//Super::PhysCustom(DeltaTime, Iterations);
}

void USMBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
    if (MovementMode == MOVE_Swimming)
    {
		bIsSwimming = true;
        CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimingCapsuleRadius, SwimingCapsuleHalfHeightSize);
        //TODO set to swiming capsule size
    }
    else if (PreviousMovementMode == MOVE_Swimming)
    {
		bIsSwimming = false;
        ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
        CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
        //TODO set normal capsule size
    }

    if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
    {
        CurrentLadder = nullptr; // очищаем указатель на текущую лестницу, когда с неё слезаем
    }

	if (PreviousCustomMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		CurrentZipline = nullptr;
	}

    if (MovementMode == MOVE_Custom)
    {
	    switch (CustomMovementMode) // оператор выбора, хороший вариант на замену if else
	    {
            case (uint8)ECustomMovementMode::CMOVE_Mantling:
            {

                GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &USMBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
                break;
            }
			default:
				break;
	    }
    }
}

ASMBaseCharacter* USMBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<ASMBaseCharacter*>(CharacterOwner);
}

void FSavedMove_SM::Clear()
{
	Super::Clear();
	bSavedIsSpreenting = 0;
	bSavedIsMantling = 0;
}

uint8 FSavedMove_SM::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	/*
	FLAG_Reserved_1 = 0x04,	// Reserved for future use
	FLAG_Reserved_2 = 0x08,	// Reserved for future use
	// Remaining bit masks are available for custom flags.
	FLAG_Custom_0 = 0x10, - Sprinting flag
	FLAG_Custom_1 = 0x20, - Mantling
	FLAG_Custom_2 = 0x40,
	FLAG_Custom_3 = 0x80,
	*/
	
	if (bSavedIsSpreenting)
	{
		Result |= FLAG_Custom_0;
	}
	if (bSavedIsMantling)
	{
		Result &= ~FLAG_JumpPressed; // снимаем флаг прыжка
		Result |= FLAG_Custom_1;
	}
	return Result;
}

bool FSavedMove_SM::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_SM* NewMove = StaticCast<FSavedMove_SM*>(NewMovePtr.Get());

	if (bSavedIsSpreenting != NewMove->bSavedIsSpreenting
	|| bSavedIsMantling != NewMove->bSavedIsMantling) // не можем комбинировать движения, если изменилось состояние подтягивания
	{
		return false;
	}
	return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

void FSavedMove_SM::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	check(Character->IsA<ASMBaseCharacter>());
	ASMBaseCharacter* InBaseCharacter = StaticCast<ASMBaseCharacter*>(Character);
	USMBaseCharacterMovementComponent* MovementComponent = InBaseCharacter->GetBaseCharacterMovementComponent();

	bSavedIsSpreenting = MovementComponent->bIsSprinting;
	bSavedIsMantling = InBaseCharacter->bIsMantling;
}

void FSavedMove_SM::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	USMBaseCharacterMovementComponent* MovementComponent = Cast<USMBaseCharacterMovementComponent>(Character->GetMovementComponent());

	MovementComponent->bIsSprinting = bSavedIsSpreenting;
}

FNetworkPredictionData_Client_Character_SM::FNetworkPredictionData_Client_Character_SM(const UCharacterMovementComponent& ClientMovement)
	:Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_Character_SM::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_SM());
}
