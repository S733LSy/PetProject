// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class S733LSYMAINPROJECT_API ASMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override; // необходимая перегрузка метода, где будет использоваться указатель на класс персонажа

	bool GetIgnoreCameraPitch() const;

	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	virtual void SetupInputComponent() override; // Перегрузка метода для установки ввода (настроки ввода с клавиатуры, мыши, геймпада)

private:
	void MoveForward(float Value); // методы перемещения
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void ChangeCrouchState();
	void Mantle();
	void Jump();

	void StartSprint();
	void StopSprint();

	void PlayerStartFire();
	void PlayerStopFire();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void StartAiming();
	void StopAiming();

	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();
	void SwitchFireMode();

	void Reload();

	void ChangeProneState();

	void Slide();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	void InteractWithZipline();

	void ToggleMainMenu();

	void Interact();

	void UseInventory();

	void ConfirmWeaponWheelSelection();

	void QuickSaveGame();
	void QuickLoadGame();

	TSoftObjectPtr<class ASMBaseCharacter> CachedBaseCharacter; // создание указателя на класс персонажа

	class USMBaseCharacterMovementComponent* SMBaseCharacterMovementComponent;
private:
	void OnInteractableObjectFound(FName ActionName);

	void CreateAndInitializeWidgets();

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;
	UUserWidget* MainMenuWidget = nullptr;

	bool bIgnoreCameraPitch = false;
};
