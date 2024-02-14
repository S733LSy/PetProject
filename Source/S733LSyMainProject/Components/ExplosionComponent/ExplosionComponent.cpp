
#include "ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"

UExplosionComponent::UExplosionComponent()
{
	SetIsReplicatedByDefault(true);
}

void UExplosionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaxDamage,
		MinDamage,
		GetComponentLocation(),
		InnerRadius,
		OuterRadius,
		DamageFallOff,
		DamageTypeClass,
		IgnoredActors,
		GetOwner(),
		Controller,
		ECC_Visibility
	);

	if (IsValid(ExplosionVFX))
	{
		NetMulticast_Explode();
	}

	if (OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
}

void UExplosionComponent::NetMulticast_Explode_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation()); // спавн взрыва, GetComponentLocation() потому что UExplosionComponent наследуется от SceneComponent
}
