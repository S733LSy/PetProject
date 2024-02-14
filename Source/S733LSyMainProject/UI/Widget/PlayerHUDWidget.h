// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetCharacterAttributes.h"
#include "PlayerHUDWidget.generated.h"

class UHighlightInterectable;
UCLASS()
class S733LSYMAINPROJECT_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UReticleWidget* GetReticleWidget() const;

	class UAmmoWidget* GetAmmoWidget() const;

	class UWidgetCharacterAttributes* GetCharacterAttributesWidget() const;

	class UThrowableAmmoWidget* GetThrowableAmmo() const;

	void SetHighlightInteractableVisibility(bool bIsVisible);
	void SetHighlightInteractableActionText(FName KeyName);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName CharacterAttributesWidgetName;

	UPROPERTY(meta = (BindWidget))
	UHighlightInterectable* InterectableKey;
};
