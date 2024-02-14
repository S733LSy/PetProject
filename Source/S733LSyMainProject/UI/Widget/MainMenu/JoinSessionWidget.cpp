// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinSessionWidget.h"

#include "SMGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<USMGameInstance>());
	SMGameInstance = StaticCast<USMGameInstance*>(GetGameInstance());
}

void UJoinSessionWidget::FindOnlineSession()
{
	SMGameInstance->OnMatchFound.AddUFunction(this, FName("OnMatchFound"));
	SMGameInstance->FindAMatch(bIsLAN);
	SearchingSessionState = ESearchingSessionState::Searching;
}

void UJoinSessionWidget::JoinOnlineSession()
{
	SMGameInstance->JoinOnlineGame();
}

void UJoinSessionWidget::CloseWidget()
{
	SMGameInstance->OnMatchFound.RemoveAll(this);
	Super::CloseWidget();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccessful)
{
	SearchingSessionState = bIsSuccessful ? ESearchingSessionState::SessionIsFound : ESearchingSessionState::None;
	SMGameInstance->OnMatchFound.RemoveAll(this);
}
