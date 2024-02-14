
#include "SMProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ASMProjectile::ASMProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	SetRootComponent(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	ProjectileMovementComponent->bAutoActivate = false;
	
	SetReplicates(true);
	AActor::SetReplicateMovement(true);
}

void ASMProjectile::LaunchProjectile(FVector Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	OnProjectileLaunched();
}

void ASMProjectile::SetProjectileActive_Implementation(bool bIsProjectileActive)
{
	ProjectileMovementComponent->SetActive(bIsProjectileActive);
}

void ASMProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this, &ASMProjectile::OnCollisionHit);
}

void ASMProjectile::OnProjectileLaunched()
{
}

void ASMProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OnProjectileHitEvent.IsBound())
	{
		OnProjectileHitEvent.Broadcast(this, Hit, ProjectileMovementComponent->Velocity.GetSafeNormal());
	}
}
