// Fill out your copyright notice in the Description page of Project Settings.


#include "HostSessionWidget.h"
#include "SMGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<USMGameInstance>());
	USMGameInstance* SMGameInstance = StaticCast<USMGameInstance*>(GetGameInstance());

	SMGameInstance->LaunchLobby(4, ServerName, bIsLAN);
}
