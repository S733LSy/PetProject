// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "S733LSyMainProjectTypes.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

APlatformTrigger::APlatformTrigger()
{
	bReplicates = true;
	NetUpdateFrequency = 2.0f; // раз в секунду (по дефолту 100)
	MinNetUpdateFrequency = 2.0f; // раз в секунду
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	SetRootComponent(TriggerBox);
}

void APlatformTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlatformTrigger, bIsActivated); // (тип дл€ которого делаетс€ репликаци€; свойство, которое реплицируем)
}

void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapEnd);
}

void APlatformTrigger::SetIsActivated(bool bIsActivated_In)
{
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated_In);
	}
}

void APlatformTrigger::OnRep_IsActivated(bool bIsActivated_Old)
{
	SetIsActivated(bIsActivated);
}

void APlatformTrigger::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASMBaseCharacter* OtherPawn = Cast<ASMBaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	// Ќадо убедитьс€, чтобы логика вызывалась только на сервере, с сервера придЄт некоторое оповещение и которое мы получим спуст€ некоторое врем€
	if (/*OtherPawn->IsLocallyControlled() || *//*GetLocalRole() == ROLE_Authority*/OtherPawn->IsLocallyControlled()) // IsLocallyControlled дл€ быстрой логики, но триггер только дл€ повнов
	{
		OverlappedPawns.AddUnique(OtherPawn);

		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			bIsActivated = true;
			SetIsActivated(true);
		}
	}
}

void APlatformTrigger::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASMBaseCharacter* OtherPawn = Cast<ASMBaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (/*OtherPawn->IsLocallyControlled() || *//*GetLocalRole() == ROLE_Authority*/OtherPawn->IsLocallyControlled())
	{
		OverlappedPawns.RemoveSingleSwap(OtherPawn);

		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			bIsActivated = false;
			SetIsActivated(false);
		}
	}
}
