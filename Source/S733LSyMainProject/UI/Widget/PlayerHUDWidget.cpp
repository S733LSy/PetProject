// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "ReticleWidget.h"
#include "Blueprint/WidgetTree.h"
#include "AmmoWidget.h"
#include "HighlightInterectable.h"

UReticleWidget* UPlayerHUDWidget::GetReticleWidget() const
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget() const
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetCharacterAttributesWidget() const
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(CharacterAttributesWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{
	if (!IsValid(InterectableKey))
	{
		return;
	}

	if (bIsVisible)
	{
		InterectableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InterectableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if (IsValid(InterectableKey))
	{
		InterectableKey->SetActionText(KeyName);
	}
}
