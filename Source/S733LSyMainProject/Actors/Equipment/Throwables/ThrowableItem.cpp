
#include "ThrowableItem.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "Characters/SMBaseCharacter.h"
#include "Characters/Controllers/SMPlayerController.h"
#include "Net/UnrealNetwork.h"

AThrowableItem::AThrowableItem()
{
	SetReplicates(true);
}

void AThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThrowableItem, ThrowableItemsPool);
	DOREPLIFETIME(AThrowableItem, CurrentThrowableItemIndex);
}

void AThrowableItem::Throw()
{
	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	ASMBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner->IsPlayerControlled())
	{
		APlayerController* Controller = CharacterOwner->GetController<ASMPlayerController>();
		if (IsValid(Controller))
		{
			Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation); // We write in the variables where the player is looking
		}
		else
		{
			PlayerViewPoint = CharacterOwner->GetActorLocation();
			PlayerViewRotation = CharacterOwner->GetBaseAimRotation();
		}
	}
	else
	{
		PlayerViewPoint = CharacterOwner->GetActorLocation();
		PlayerViewRotation = CharacterOwner->GetBaseAimRotation();
	}

	if (CharacterOwner->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint); // 4x4 transformation matrix

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable); // getting the socket position in global coordinates
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation); // Translation from world coordinates to local coordinates
	
	ASMProjectile* Projectile = ThrowableItemsPool[CurrentThrowableItemIndex];
	Projectile->SetActorLocation(PlayerViewPoint + ViewDirection * SocketInViewSpace.X); // The X coordinate, because the distance from the camera is calculated from it
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->SetProjectileActive(true);
	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	++CurrentThrowableItemIndex;

	if (CurrentThrowableItemIndex == ThrowableItemsPool.Num())
	{
		CurrentThrowableItemIndex = 0;
	}
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(ProjectileClass))
	{
		return;
	}
	ThrowableItemsPool.Reserve(ThrowablePoolSize);

	for (int32 i = 0; i < ThrowablePoolSize; ++i)
	{
		ASMProjectile* Projectile = GetWorld()->SpawnActor<ASMProjectile>(ProjectileClass, ThrowablePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetCharacterOwner());
		Projectile->SetProjectileActive(false);
		ThrowableItemsPool.Add(Projectile);
	}
}

EAmunitionType AThrowableItem::GetAmmoType() const
{
	return AmmoType;
}