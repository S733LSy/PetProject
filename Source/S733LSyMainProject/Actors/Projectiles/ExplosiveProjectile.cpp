// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"

#include "Characters/SMBaseCharacter.h"
#include "Components/ExplosionComponent/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("Explosion Component"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
	SetReplicates(true);
}

void AExplosiveProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationTime, false); // Какой таймер запускаем, у какого объекта, какой метод сработает по завершении, сколько будет длиться таймер, зацикленный или нет
}

void AExplosiveProjectile::OnDetonationTimerElapsed()
{
	ExplosionComponent->Explode(GetController());
}

AController* AExplosiveProjectile::GetController() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}
