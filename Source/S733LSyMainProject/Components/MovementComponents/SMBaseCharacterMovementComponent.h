// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S733LSyMainProject/Characters/SMBaseCharacter.h"
#include "SMBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	UPrimitiveComponent* TargetActor;

	FVector InitialLocation = FVector::ZeroVector; // ��������� ��������������
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector; // �������� ��������������
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f; // ������������ ������������
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Zipline UMETA(Displayname = "Zipline"),
	CMOVE_WallRun UMETA(DisplayName = "WallRun"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};
/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API USMBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_SM; // FSavedMove_SM ����� ����� ������ �� ���� ������ ������ USMBaseCharacterMovementComponent

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override; // ����������� ������

	virtual void PhysicsRotation(float DeltaTime) override; // ����� ���������� �� �������� ���������

	FORCEINLINE bool GetIsJumpCalled() { return bIsJumpCalled; }
	void SetIsJumpCalled()
	{
		if (GetIsProne() == true)
		{
			bIsJumpCalled = !bIsJumpCalled;
		}
	}
	FORCEINLINE bool GetIsProne() { return bIsProne; }
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	FORCEINLINE bool IsSwimming() const { return bIsSwimming; }
	virtual float GetMaxSpeed() const override;
	void SetIsOutOfStamina(bool bIsOutOfStamina_Im);
	void StartSprint();
	void StopSprint();
	bool bEnProne() const;

	void Prone();

	void UnProne();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters); //LedgeDescription �������� ����� �� ������� �� ����� ���������
	void EndMantle();
	bool IsMantling() const;

	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder();

	void AttachToZipline(const class AZiplineInteractiveActor* Zipline);
	void DetachFromZipline();
	float GetActorToCurrentZiplineProjection(const FVector& Location) const;
	bool IsOnZipline() const;
	const class AZiplineInteractiveActor* GetCurrentZipline();

	float GetLadderSpeedRatio() const;
	void StartSlideMeshOffset();

	void StartSlide();
	void EndSlide();
	bool GetIsSliding();

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations); // �������� �� ������������
	void PhysLadder(float DeltaTime, int32 Iterations); // �������� �� ������������ �� ��������
	void PhysZipline(float DeltaTime, int32 Iterations);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: OutOfStamina", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Prone")
	float ProneCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Prone")
	float ProneCapsuleHalfHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Prone")
	float MaxProneSpeed = 100.0f;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimingCapsuleRadius = 60.0f;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimingCapsuleHalfHeightSize = 60.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderBrakingDeceleration = 2048.0f; // ������������ ��������

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float LadderToCharacterOffset = 60.0f; // ����������� ������, ����� ������� ���������� ��������� �� ��������

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxLadderTopOffset = 90.0f; // ��� ����, ����� ������ (�������� ������� ���������), ����� ���� ��������

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MinLadderBottomOffset = 90.0f; // ����� ��� ��������

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float JumpOffFromLadderSpeed = 500.0f; // �������� ������������, �� ��� ��� ������ ���������� ������, �� ����� �������, ��� ��� �����
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float JumpUpOffLadderHeight = 500.0f; // ������ ������ �����, ��� ������ � ��������

	UPROPERTY(Category = "Character Movement: Zipline", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxZiplineSpeed = 200.0f;
	UPROPERTY(Category = "Character Movement: Zipline", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineToCharacterOffset = 60.0f;

	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideSpeed = 1000.0f;
	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideCaspsuleHalfHeight = 60.0f;
	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideMaxTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement | Capsule size")
	float SlideCapsuleHalfHeight = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement | Capsule size")
	float SlideOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations | Slide")
	UAnimMontage* SlideAnimMontage;


	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	class ASMBaseCharacter* GetBaseCharacterOwner() const; // ����� ���� ������ ���������

private:
	bool bIsOutOfStamina;
	bool bIsSprinting;
	bool bIsProne = false;
	bool bIsJumpCalled = false;
	float ProneHalfHeight = 40.0f;;
	bool bIsSwimming = false;

	FMantlingMovementParameters CurrentMantlingParameters;

	FTimerHandle MantlingTimer;

	const ALadder* CurrentLadder = nullptr;

	const AZiplineInteractiveActor* CurrentZipline = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

	bool bOrientToZipline;

	bool bIsSliding;
	FTimerHandle SlidingTimer;
};

class FSavedMove_SM : public FSavedMove_Character
{
	typedef FSavedMove_Character Super; // ��� ������������� ������ FSavedMove_Character � ���� Super

public:
	virtual void Clear() override; // ������� ��� ������������ ��������, ����� ��� ������� ������ ���������

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(ACharacter* Character) override;

private:
	uint8 bSavedIsSpreenting : 1; // 1 ���� �� 8��� ������
	uint8 bSavedIsMantling : 1;
};

class FNetworkPredictionData_Client_Character_SM : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_Character_SM(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};