// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkWidget.h"

FText UNetworkWidget::GetNetworkType() const
{
	return bIsLAN ? FText::FromString(TEXT("LAN")) : FText::FromString(TEXT("Internet")); 
}

void UNetworkWidget::ToggleNetworkType()
{
	bIsLAN = !bIsLAN;
}

void UNetworkWidget::CloseWidget()
{
	if (OnNetworkWidgetClosedEvent.IsBound())
	{
		OnNetworkWidgetClosedEvent.Broadcast();
	}
	SetVisibility(ESlateVisibility::Hidden);
}
