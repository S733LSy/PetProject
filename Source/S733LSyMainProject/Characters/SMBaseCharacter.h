
#pragma once

#include "CoreMinimal.h"
#include "S733LSyMainProjectTypes.h"
#include "Runtime\Engine\Classes\Components\TimelineComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "SubSystems/SaveSubSystem/SaveSubsystemInterface.h"
#include "UObject/ScriptInterface.h"
#include "SMBaseCharacter.generated.h"

enum class EPickableItemType : uint8;
class UCharacterInventoryComponent;
class UInventoryItem;
DECLARE_DELEGATE_OneParam(FOnIteractableObjectFound, FName);

class IInteractable;

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxHeight = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinHeight = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxHeightStartTime = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinHeightStartTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float AnimationCorrectionXY = 65.0f; // обычно примерно равн€етс€ радиусу капсулы персонажа
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float AnimationCorrectionZ = 200.0f;

};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)

class UWidgetComponent;
class AInteractiveActor;
class USMBaseCharacterMovementComponent; // forward declaration, чтобы не писать "class" перед создание указател€ или еще чего-то видимо
class UCharacterAttributeComponent;
class UCharacterEquipmentComponent;
// forward declaration позвол€ет избегать добавлени€ ненужных иклюдов и избежать циклических зависимостей при сборке (линковке) проекта
UCLASS(Abstract, NotBlueprintable) // abstract говорит о том, что мы не можеи дистанцировать данный класс на сцене через pawn или drag and drop'ом через меню
class S733LSYMAINPROJECT_API ASMBaseCharacter : public ACharacter, public IGenericTeamAgentInterface, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	ASMBaseCharacter(const FObjectInitializer& ObjectInitializer); // выставл€ем нужный movement component

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//@ ISaveSubsystemInterface
	virtual void OnLevelDeserialized_Implementation() override;
	//~ISaveSubsystemInterface

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void MoveForward(float Value) {}; // не определены методы передвижени€, дл€ стабильного интерфейса, управление может быть игроком или »», работа с базовыми классами персонажа производитс€ через контроллер
	virtual void MoveRight(float Value) {}; // така€ же пуста€ реализаци€
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};
	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	bool IsSwimmingUnderWater() const;


	virtual void SetbIsProneRequested() { bIsProneRequested = false; }

	bool GetIsProned() { return bIsProned; }

	virtual void ChangeCrouchState();
	virtual void ChangeProneState() {};


	virtual void StartSprint();
	virtual void StopSprint();
	virtual void IsOutOfStamina(bool bIsOutOfStamina);

	virtual void OnStartProne() {};
	virtual void OnEndProne() {};
	virtual void ProneToIdleState();

	virtual void Slide();

	UFUNCTION(BlueprintCallable)
	void Mantle(bool bForce = false);

	UPROPERTY(ReplicatedUsing = OnRep_IsMantling)
	bool bIsMantling;

	UFUNCTION()
	void OnRep_IsMantling(bool bWasMantling);

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void Tick(float DeltaTime) override;

	void StartFire();
	void StopFire();

	void StartAiming();
	void StopAiming();

	FRotator GetAimOffset();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	float GetAimingMovementSpeed() const;

	bool IsAiming() const;

	void Reload();

	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();
	void SwitchFireMode();

	FOnAimingStateChanged OnAimingStateChanged;

	void TryChangeSprintState(float DeltaTime);

	FORCEINLINE USMBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return SMBaseCharacterMovementComponent; }

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const; // получение компонента, который объ€влен в protected и создан в методе ASMBaseCharacter::ASMBaseCharacter(const FObjectInitializer& ObjectInitializer)

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;

	UCharacterAttributeComponent* GetCharacterAttributeComponent() const;

	void SetIsOutOfStamina();
	FORCEINLINE float GetCurrentStamina() { return CurrentStamina; }

	FORCEINLINE bool GetIsPressed() { return bIsButtonPressed; }

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor);

	void ClimbLadderUp(float Value);

	void ZiplineSlideDown(float Value);
	void InteractWithLadder();
	const class ALadder* GetAvailableLadder() const; // метод дл€ поиска подход€щего объекта

	void InteractWithZipline();
	const class AZiplineInteractiveActor* GetAvailableZipline() const;

	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void HardLanding();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void Interact();

	FOnIteractableObjectFound OnIteractableObjectFound;

	bool PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup, int32 Count_In, EPickableItemType ItemType);
	void UseInventory(APlayerController* PlayerController);

	void ConfirmWeaponSelection();

	/** IGenericTeamAgentInterface */
	virtual FGenericTeamId GetGenericTeamId() const override;
	/** ~IGenericTeamAgentInterface */

	UPROPERTY(VisibleDefaultsOnly, Category = "Character | Components")
	UWidgetComponent* HealthBarProgressComponent;

	void InitializeHealthProgress();

	UCharacterInventoryComponent* GetCharacterInventoryComponent() const;

protected:

	//UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement") // добавл€ет возможность вызова функции в блупринтах персонажа
	virtual void OnSprintStart_Implementation();

	//UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement") // добавл€ет возможность вызова функции в блупринтах персонажа
	virtual void OnSprintEnd_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Stamina")
	float JumpZVelocity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | HardLanding")
	float HardLandingMinimalHeight = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Character | Movement | HardLanded")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Character | Components")
	UCharacterAttributeComponent* CharacterAttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Character | Components")
	UCharacterInventoryComponent* CharacterInventoryComponent;

	virtual bool CanSprint();

	bool CanMantle() const;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);

	virtual void OnHardLanding();

	USMBaseCharacterMovementComponent* SMBaseCharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HeightMantleSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantleSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LowMantleMaxHeight = 125.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | HardLanded")
	UAnimMontage* BaseCharacterHardLandingAnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | HardLanded")
	UAnimMontage* FPBaseCharacterHardLandingAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Swimming | Oxygen")
	float WaterVolumeDamage = 2.0f;

	void EnableMovement();

	virtual void OnOutOfOxygen(); // метод, который мы будем подписывать на событие делегата DECLARE_MULTICAST_DELEGATE(FOutCharacterOxygenEventSignature); FOutCharacterOxygenEventSignature OutOfOxygen;
	virtual void OnDeath(); // метод, который мы будем подписывать на событие делегата смерти персонажа

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	UAnimMontage* OnDeathAnimMontage;
	// Damage depending from fall height (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	UCurveFloat* FallDamageCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Character | Components")
	UCharacterEquipmentComponent* CharacterEquipmentComponent; // переменна€ класса

	virtual void OnStartAimingInternal();

	virtual void OnStopAimingInternal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
	ETeams Team = ETeams::Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | ") // длина взгл€да
	float LineOfSightDistance = 500.0f;

	void TraceLineOfSight(); // метод, который выполн€ет поиск по линии взг€да

	UPROPERTY()
	TScriptInterface<IInteractable> LineOfSightObject;

private:
	float CurrentStamina = 0.0f;
	bool bIsAiming;
	float CurrentAimingMovementSpeed;
	bool bIsSprintRequested = false;
	bool bIsButtonPressed = false;
	bool bIsProneRequested = false;
	bool bIsProned = false;
	bool bWantToUnProne = false;
	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
	TArray<AInteractiveActor*> AvailableInteractiveActors;
	FVector TopPointFalling;
	FTimerHandle HardLandingAnimMontageTimer;
	float FallHeight;

	void EnableRagdoll();
};
