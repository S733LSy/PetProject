
#include "SMPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "UI/Widget/PlayerHUDWidget.h"
#include "UI/Widget/ReticleWidget.h"
#include "UI/Widget/AmmoWidget.h"
#include "UI/Widget/WidgetCharacterAttributes.h"
#include "S733LSyMainProjectTypes.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystems/SaveSubSystem/SaveSubsystem.h"

void ASMPlayerController::SetPawn(APawn* InPawn) 
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<ASMBaseCharacter>(InPawn); // если тип не совпадает с basepawn, то в указателе будет пустота
	if (CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndInitializeWidgets();
		CachedBaseCharacter->OnIteractableObjectFound.BindUObject(this, &ASMPlayerController::OnInteractableObjectFound);
	}
}

bool ASMPlayerController::GetIgnoreCameraPitch() const
{
	return bIgnoreCameraPitch;
}

void ASMPlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void ASMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ASMPlayerController::MoveForward); // "this" - указатель ан класс
	InputComponent->BindAxis("MoveRIght", this, &ASMPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ASMPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ASMPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &ASMPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ASMPlayerController::LookUpAtRate);
	InputComponent->BindAxis("SwimForward", this, &ASMPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &ASMPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &ASMPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &ASMPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &ASMPlayerController::InteractWithLadder);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &ASMPlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ASMPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ASMPlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ASMPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ASMPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ASMPlayerController::StopSprint);
	InputComponent->BindAction("Prone", EInputEvent::IE_Released, this, &ASMPlayerController::ChangeProneState);
	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &ASMPlayerController::Slide);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ASMPlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ASMPlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ASMPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ASMPlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ASMPlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &ASMPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &ASMPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &ASMPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("SwitchFireMode", EInputEvent::IE_Pressed, this, &ASMPlayerController::SwitchFireMode);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ASMPlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ASMPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction(ActionInteract, EInputEvent::IE_Pressed, this, &ASMPlayerController::Interact);
	InputComponent->BindAction("UseInventory", EInputEvent::IE_Pressed, this, &ASMPlayerController::UseInventory);
	InputComponent->BindAction("ConfirmWeaponWheelSelection", EInputEvent::IE_Pressed, this, &ASMPlayerController::ConfirmWeaponWheelSelection);
	InputComponent->BindAction("QuickSaveGame", EInputEvent::IE_Pressed, this, &ASMPlayerController::QuickSaveGame);
	InputComponent->BindAction("QuickLoadGame", EInputEvent::IE_Pressed, this, &ASMPlayerController::QuickLoadGame);
	FInputActionBinding& ToggleMenuBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &ASMPlayerController::ToggleMainMenu);
	ToggleMenuBinding.bExecuteWhenPaused = true;
}

void ASMPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->MoveForward(Value); } // если basepawn валидный, просто вызывается метод
}

void ASMPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->MoveRight(Value); }
}

void ASMPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->Turn(Value); }
}

void ASMPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->LookUp(Value); }
}

void ASMPlayerController::TurnAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->TurnAtRate(Value); }
}

void ASMPlayerController::LookUpAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->LookUpAtRate(Value); }
}

void ASMPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->ChangeCrouchState(); }
}

void ASMPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->Mantle(); }
}

void ASMPlayerController::Jump()
{
	if (CachedBaseCharacter->GetIsProned() == false)
	{
		if (CachedBaseCharacter.IsValid()) { CachedBaseCharacter->Jump(); }
	}
	else if (CachedBaseCharacter->GetIsProned() == true)
	{
		CachedBaseCharacter->ProneToIdleState();
	}
}

void ASMPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void ASMPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void ASMPlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void ASMPlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void ASMPlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void ASMPlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void ASMPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void ASMPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void ASMPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void ASMPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void ASMPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void ASMPlayerController::SwitchFireMode()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwitchFireMode();
	}
}

void ASMPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void ASMPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void ASMPlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Slide();
	}
}

void ASMPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void ASMPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void ASMPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void ASMPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void ASMPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void ASMPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void ASMPlayerController::ToggleMainMenu()
{
	if (!IsValid(MainMenuWidget) || !IsValid(PlayerHUDWidget))
	{
		return;
	}

	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly{});
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		MainMenuWidget->AddToViewport();
		PlayerHUDWidget->RemoveFromParent(); // убирает player hud widget
		SetInputMode(FInputModeGameAndUI{});
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void ASMPlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void ASMPlayerController::UseInventory()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->UseInventory(this);
	}
}

void ASMPlayerController::ConfirmWeaponWheelSelection()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ConfirmWeaponSelection();
	}
}

void ASMPlayerController::QuickSaveGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->SaveGame();
}

void ASMPlayerController::QuickLoadGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->LoadLastGame();
}

void ASMPlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);
	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);
}

void ASMPlayerController::CreateAndInitializeWidgets()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}

	if (CachedBaseCharacter.IsValid() && IsValid(PlayerHUDWidget))
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if (IsValid(ReticleWidget))
		{
			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnEquippedItemChangedEvent.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}
		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
			CharacterEquipment->OnCurrentThrowableItemAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateGrenadeCount"));
		}
		UWidgetCharacterAttributes* CharacterAttributesWidget = PlayerHUDWidget->GetCharacterAttributesWidget();
		if (IsValid(CharacterAttributesWidget))
		{
			UCharacterAttributeComponent* CharacterAttributes = CachedBaseCharacter->GetCharacterAttributeComponent();
			CharacterAttributes->OnStaminaUpdate.AddUFunction(CharacterAttributesWidget, FName("GetStaminaPercent"));
			CharacterAttributes->OnOxygenUpdateEvent.AddUFunction(CharacterAttributesWidget, FName("GetOxygenPercent"));
		}
	}
	SetInputMode(FInputModeGameOnly{});
	bShowMouseCursor = false;
}
