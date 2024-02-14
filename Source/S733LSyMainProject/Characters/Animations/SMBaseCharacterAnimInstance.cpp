
#include "SMBaseCharacterAnimInstance.h"

#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/MovementComponents/SMBaseCharacterMovementComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void USMBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ASMBaseCharacter>(), TEXT("USMBaseCharacterAnimInstance::NativeBeginPlay() USMBaseCharacterAnimInstance can be used only with ASMBaseCharacter"));
	// проверка на персонажа
	CachedBaseCharacter = StaticCast<ASMBaseCharacter*>(TryGetPawnOwner());
}

void USMBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	bIsAiming = CachedBaseCharacter->IsAiming();

	USMBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent(); // создаём указатель, в которой помещаем CharacterMovementComponent персонажа
	Speed = CharacterMovement->Velocity.Size(); // получение скорости
	bIsFalling = CharacterMovement->IsFalling(); // получение bIsFalling
	bIsCrouching = CharacterMovement->IsCrouching(); // получение bIsCrouching
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsProne = CharacterMovement->GetIsProne();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsOnLadder = CharacterMovement->IsOnLadder();
	if (bIsOnLadder)
	{
		LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();
	}
	bIsOnZipline = CharacterMovement->IsOnZipline();
	bIsStrafing = !CharacterMovement->bOrientRotationToMovement;
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());

	AimRotation = CachedBaseCharacter->GetAimOffset();

	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItemType = CharacterEquipment->GetCurrentEquippedItemType();

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipment->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
	}
}
