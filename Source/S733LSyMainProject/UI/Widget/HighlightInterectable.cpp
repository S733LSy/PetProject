// Fill out your copyright notice in the Description page of Project Settings.


#include "HighlightInterectable.h"
#include "Components/TextBlock.h"

void UHighlightInterectable::SetActionText(FName KeyName)
{
	if (IsValid(ActionText))
	{
		ActionText->SetText(FText::FromName(KeyName));
	}
}
