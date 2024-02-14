// Fill out your copyright notice in the Description page of Project Settings.
#include "SMBaseCharacter.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MovementComponents/SMBaseCharacterMovementComponent.h"
#include "Components/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "Actors/Intaractive/InteractiveActor.h"
#include "Actors/Intaractive/Environment/Ladder.h"
#include "Actors/Intaractive/Environment/ZiplineInteractiveActor.h"
#include "GameFramework/PhysicsVolume.h"
#include "S733LSyMainProjectTypes.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Intaractive/Interface/Interactive.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Controllers/SMPlayerController.h"
#include "Inventory/Items/InventoryItem.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/World/SMAttributeProgressBar.h"

ASMBaseCharacter::ASMBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMBaseCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName)) // ���������� ������ movement component
{
	SMBaseCharacterMovementComponent = StaticCast<USMBaseCharacterMovementComponent*>(GetCharacterMovement());

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributeComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharacterAttributes"));
	CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("CharacterInventoryComponent"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));

	HealthBarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarProgressComponent"));
	HealthBarProgressComponent->SetupAttachment(GetCapsuleComponent());
}

void ASMBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterAttributeComponent->OutOfOxygen.AddUObject(this, &ASMBaseCharacter::OnOutOfOxygen);
	CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &ASMBaseCharacter::OnDeath);
	CharacterAttributeComponent->OutOfStaminaEvent.AddUObject(this, &ASMBaseCharacter::IsOutOfStamina);
	JumpZVelocity = SMBaseCharacterMovementComponent->JumpZVelocity;
	InitializeHealthProgress();
}

void ASMBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OnIteractableObjectFound.IsBound())
	{
		OnIteractableObjectFound.Unbind();
	}
	Super::EndPlay(EndPlayReason);
}

void ASMBaseCharacter::OnLevelDeserialized_Implementation()
{
}

void ASMBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMBaseCharacter, bIsMantling);
}

void ASMBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

bool ASMBaseCharacter::IsSwimmingUnderWater() const
{
	FVector HeadPosition = GetMesh()->GetSocketLocation(FName("head"));
	if (GetBaseCharacterMovementComponent()->IsSwimming())
	{
		APhysicsVolume* Volume = GetBaseCharacterMovementComponent()->GetPhysicsVolume();
		// �������� ������ ���������� �����
		float VolumeTopPlane = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->
			GetActorScale3D().Z;
		// ����� Z ���������� ��� ������ ������� ������ � ��������� � ���� �������� ������ ����� � ������ ��������
		if (VolumeTopPlane < HeadPosition.Z)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void ASMBaseCharacter::ChangeCrouchState()
{
	if (GetCharacterMovement()->IsCrouching() && bIsProneRequested == false)
	{
		UnCrouch();
	}
	else if (GetCharacterMovement()->IsCrouching() == false && bIsProneRequested == false)
	{
		Crouch();
		bIsProneRequested = true;
	}
	else if (SMBaseCharacterMovementComponent->GetIsProne() == true)
	{
		if (SMBaseCharacterMovementComponent->bEnProne() == false)
		{
			OnEndProne();
			SMBaseCharacterMovementComponent->UnProne();
			bIsProneRequested = false;
			bIsProned = false;
		}
		else
		{
			bWantToUnProne = true;
			return;
		}
	}
	else if (GetCharacterMovement()->IsCrouching() && bIsProneRequested == true)
	{
		OnStartProne();
		SMBaseCharacterMovementComponent->Prone();
		bIsProned = true;
	}
}

void ASMBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsProned)
	{
		OnEndProne();
		SMBaseCharacterMovementComponent->UnProne();
		UnCrouch();
		bIsProneRequested = false;
	}
	else if (bIsCrouched)
	{
		UnCrouch();
		bIsProneRequested = false;
	}
}

void ASMBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void ASMBaseCharacter::IsOutOfStamina(bool bIsOutOfStamina)
{
	SMBaseCharacterMovementComponent->SetIsOutOfStamina(bIsOutOfStamina);
}

void ASMBaseCharacter::ProneToIdleState()
{
	OnEndProne();
	SMBaseCharacterMovementComponent->UnProne();
	UnCrouch();
	bIsProneRequested = false;
	bIsProned = false;
}

void ASMBaseCharacter::Slide()
{
	GetBaseCharacterMovementComponent()->StartSlide();
}

void ASMBaseCharacter::Mantle(bool bForce /*= false*/)
{
	if (!(CanMantle() || bForce))
	{
		return;
	}

	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription) && !GetBaseCharacterMovementComponent()->IsMantling())
	{
		bIsMantling = true;

		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location - LedgeDescription.AbsLocation;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.TargetActor = LedgeDescription.Component;

		float MantlingHeight = (LedgeDetectorComponent->GetDownwardCheckHitResult() - LedgeDetectorComponent->
			GetCharacterBottomLocation()).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);
		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);

		MantlingParameters.Duration = MaxRange - MinRange;
		// StartTime ��������� � ���������� ����� ������������ � ����������� �������� �����, ��� � ������� �������� ������ �� ������� �� ����� ��������� ����� ������� ������������ � ����������� �������

		//float StartTime = MantlingSettings.MaxHeightStartTime + (MantlingHeight - MantlingSettings.MinHeight) / (MantlingSettings.MaxHeight - MantlingSettings.MinHeight) * (MantlingSettings.MaxHeightStartTime - MantlingSettings.MinHeightStartTime);
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;
		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
		{
			GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);
		}

		UnCrouch();
		SetbIsProneRequested();


		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}

void ASMBaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && !bWasMantling && bIsMantling)
	{
		Mantle(true);
	}
}

bool ASMBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
	// ���� ������ ������������� ��������������, �� ������ � const
}

void ASMBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TraceLineOfSight();
	TryChangeSprintState(DeltaTime);
	if (bWantToUnProne)
	{
		if (SMBaseCharacterMovementComponent->GetIsProne())
		{
			OnEndProne();
			SMBaseCharacterMovementComponent->UnProne();
			bIsProneRequested = false;
			bIsProned = false;
			bWantToUnProne = false;
			SMBaseCharacterMovementComponent->JumpZVelocity = JumpZVelocity;
		}
	}
}

void ASMBaseCharacter::StartFire()
{
	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		return;
	}

	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void ASMBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void ASMBaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
	CurrentRangeWeapon->StartAim();
	OnStartAiming();
}

void ASMBaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}

	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	OnStopAiming();
}

FRotator ASMBaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	FRotator Result = AimDirectionLocal.ToOrientationRotator();

	return Result;
}

void ASMBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void ASMBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

float ASMBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

bool ASMBaseCharacter::IsAiming() const
{
	return bIsAiming;
}

void ASMBaseCharacter::Reload()
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void ASMBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void ASMBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void ASMBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}

void ASMBaseCharacter::SwitchFireMode()
{

}

const UCharacterEquipmentComponent* ASMBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

UCharacterEquipmentComponent* ASMBaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

UCharacterAttributeComponent* ASMBaseCharacter::GetCharacterAttributeComponent() const
{
	return CharacterAttributeComponent;
}

void ASMBaseCharacter::SetIsOutOfStamina()
{

}

void ASMBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor); // ���������� � ������ interactive
}

void ASMBaseCharacter::UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor); // �������� �� ������� interactive
}

void ASMBaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void ASMBaseCharacter::ZiplineSlideDown(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		FVector ZiplineForwardVector = GetBaseCharacterMovementComponent()->GetCurrentZipline()->GetActorForwardVector();
	}
}

void ASMBaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		// ������ ��������� ������ �� ����� �������� � ����������� ��� ������ ���������� ��������
		if (IsValid(AvailableLadder)) // ���� ����� ����� ������� ��
		{
			if (AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder); // �� ������������� � ���
		}
	}
}

const ALadder* ASMBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	// ����� �� ��������� ������������� ��������
	{
		if (InteractiveActor->IsA<ALadder>()) // ���� ����� �������� ��������� ��
		{
			Result = StaticCast<const ALadder*>(InteractiveActor); // ���������� ��� � ���������
			break; // ����� ����� ������ ���������� �������, ������� �� �����
		}
	}
	return Result;
}

void ASMBaseCharacter::InteractWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZiplineInteractiveActor* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline))
		{
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

const AZiplineInteractiveActor* ASMBaseCharacter::GetAvailableZipline() const
{
	const AZiplineInteractiveActor* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZiplineInteractiveActor>())
		{
			Result = StaticCast<const AZiplineInteractiveActor*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void ASMBaseCharacter::Falling()
{
	Super::Falling();
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void ASMBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	TopPointFalling = GetActorLocation();
}

void ASMBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	FallHeight = (TopPointFalling - GetActorLocation()).Z * 0.01f;

	if (FallHeight > HardLandingMinimalHeight * 0.01f && IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
		if (CharacterAttributeComponent->IsAlive())
		{
			HardLanding();
		}
	}
}

void ASMBaseCharacter::HardLanding()
{
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(BaseCharacterHardLandingAnimMontage);

	GetWorld()->GetTimerManager().SetTimer(HardLandingAnimMontageTimer, this, &ASMBaseCharacter::EnableMovement,
	                                       Duration, false);
	OnHardLanding();
}

void ASMBaseCharacter::PrimaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
	}
}

void ASMBaseCharacter::SecondaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
	}
}

void ASMBaseCharacter::Interact()
{
	if (LineOfSightObject.GetInterface())
	{
		LineOfSightObject->Interact(this);
	}
}

bool ASMBaseCharacter::PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup, int32 Count_In, EPickableItemType ItemType)
{
	bool Result = false;

	if (CharacterInventoryComponent->HasFreeSlot())
	{
		CharacterInventoryComponent->AddItem(ItemToPickup, Count_In, ItemType);
		Result = true;
	}
	return Result;
}

void ASMBaseCharacter::UseInventory(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (!CharacterInventoryComponent->IsViewVisible())
	{
		CharacterInventoryComponent->OpenViewInventory(PlayerController);
		CharacterEquipmentComponent->OpenViewEquipment(PlayerController);
		PlayerController->SetInputMode(FInputModeGameAndUI{});
		PlayerController->bShowMouseCursor = true;
	}
	else
	{
		CharacterInventoryComponent->CloseViewInventory();
		CharacterEquipmentComponent->CloseViewEquipment();
		PlayerController->SetInputMode(FInputModeGameOnly{});
		PlayerController->bShowMouseCursor = false;
	}
}

void ASMBaseCharacter::ConfirmWeaponSelection()
{
	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		CharacterEquipmentComponent->ConfirmWeaponSelection();
	}
}

FGenericTeamId ASMBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
}

void ASMBaseCharacter::OnHardLanding()
{
}

void ASMBaseCharacter::InitializeHealthProgress()
{
	USMAttributeProgressBar* Widget = Cast<USMAttributeProgressBar>(HealthBarProgressComponent->GetUserWidgetObject());
	if (!IsValid(Widget))
	{
		HealthBarProgressComponent->SetVisibility(false);
		return;
	}

	if (IsPlayerControlled() && IsLocallyControlled())
	{
		HealthBarProgressComponent->SetVisibility(false);
	}

	CharacterAttributeComponent->OnHealthChangedEvent.AddUObject(Widget, &USMAttributeProgressBar::SetProgressPercantage);
	CharacterAttributeComponent->OnDeathEvent.AddLambda([=]() {HealthBarProgressComponent->SetVisibility(false);});
	Widget->SetProgressPercantage(CharacterAttributeComponent->GetHealthPercent());
}

UCharacterInventoryComponent* ASMBaseCharacter::GetCharacterInventoryComponent() const
{
	return CharacterInventoryComponent;
}

void ASMBaseCharacter::OnSprintStart_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("ASMBaseCharacter::OnSprintStart_Implementation"))
}

void ASMBaseCharacter::OnSprintEnd_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ASMBaseCharacter::OnSprintEnd_Implementation"))
}

bool ASMBaseCharacter::CanSprint()
{
	return !SMBaseCharacterMovementComponent->IsOutOfStamina() && !SMBaseCharacterMovementComponent->IsFalling() && !
		SMBaseCharacterMovementComponent->IsOnLadder() && !SMBaseCharacterMovementComponent->IsOnZipline();
}

bool ASMBaseCharacter::CanMantle() const
{
	return !(GetBaseCharacterMovementComponent()->IsOnLadder() || GetBaseCharacterMovementComponent()->IsOnZipline() || GetBaseCharacterMovementComponent()->IsMantling());
}

void ASMBaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
}

void ASMBaseCharacter::EnableMovement()
{
	GetCharacterMovement()->SetDefaultMovementMode();
}

void ASMBaseCharacter::OnOutOfOxygen()
{
	TakeDamage(WaterVolumeDamage, FDamageEvent(), GetController(), GetOwner());
}

void ASMBaseCharacter::OnDeath()
{
	if (GetBaseCharacterMovementComponent()->IsSwimming())
	{
		GetCharacterMovement()->DisableMovement();
		EnableRagdoll();
		return;
	}
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.0f)
	{
		EnableRagdoll();
	}
}

void ASMBaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void ASMBaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void ASMBaseCharacter::TraceLineOfSight()
{
	if (!IsPlayerControlled())
	{
		return;
	}

	FVector ViewLocation; // ��� �� �������� ������� ����������
	FRotator ViewRotation;

	APlayerController* PlayerController = GetController<APlayerController>();
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation); // ��������� ����� ���� ������� �����

	FVector ViewDirection = ViewRotation.Vector(); // �������� ������ �����������
	FVector TraceEnd = ViewLocation + ViewDirection * LineOfSightDistance; // �������� ����� ����������

	FHitResult HitResult; // ��������� �����
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility); // ECC_Visibility - ������ �� �������, ������� �����
	if (LineOfSightObject.GetObject() != HitResult.Actor)
	{
		LineOfSightObject = HitResult.Actor.Get();

		FName ActionName;
		if (LineOfSightObject.GetInterface())
		{
			ActionName = LineOfSightObject->GetActionEventName();
		}
		else
		{
			ActionName = NAME_None;
		}
		OnIteractableObjectFound.ExecuteIfBound(ActionName);
	}
}

const FMantlingSettings& ASMBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HeightMantleSettings : LowMantleSettings;
}

void ASMBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}

void ASMBaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if (bIsSprintRequested && !SMBaseCharacterMovementComponent->IsSprinting() && CanSprint())
	// ���� ���� ������ �� ��� � �������� �� ����� � ��� ���� ����� ������, ��
	{
		SMBaseCharacterMovementComponent->StartSprint();
		OnSprintStart_Implementation();
	}
	if (!bIsSprintRequested && SMBaseCharacterMovementComponent->IsSprinting())
	// ���� ������� �� ��� ���, �� ��� ���� �������� �����, ��
	{
		SMBaseCharacterMovementComponent->StopSprint();
		OnSprintEnd_Implementation();
	}
}
