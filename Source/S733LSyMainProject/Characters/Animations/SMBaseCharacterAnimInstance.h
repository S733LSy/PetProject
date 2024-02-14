
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "S733LSyMainProjectTypes.h"
#include "SMBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API USMBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; // ���������� ��������� ������� � ���������� ������ - DeltaSeconds
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation", meta = (UIMin = 0.0f, UIMax = 500.0f))
	float Speed = 0.0f; // ������ ������� ��������, ������� ��������� ��������

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsFalling = false; // ��� ������, ����, ������� ���������� ��������� �������� � ������� ��� ���

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsCrouching = false; // ��� ����������, ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSprinting = false; // ��� ����, ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOutOfStamina = false; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsProne = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSwimming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOnLadder = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float LadderSpeedRatio = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOnZipline = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsStrafing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation", meta = (UIMin = -180.0f, UIMax = 180.0f))
	float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	EEquipableItemType CurrentEquippedItemType = EEquipableItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation | Weapon")
	FTransform ForeGripSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation | Weapon")
	bool bIsAiming;

private:
	TWeakObjectPtr<class ASMBaseCharacter> CachedBaseCharacter;
};
