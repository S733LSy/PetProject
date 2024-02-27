
#include "AITurretController.h"
#include "AI/Characters/Turret.h"
#include "GameFramework/Character.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

void AAITurretController::BeginPlay()
{
	Super::BeginPlay();

	CachedTurret->OnTakeAnyDamage.AddDynamic(this, &AAITurretController::OnTakeDamage);
}

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn() AAITurretController can possess only Turrets"));
		CachedTurret = Cast<ATurret>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

	if (!CachedTurret.IsValid())
	{
		return;
	}

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Damage::StaticClass());
	if (!IsValid(ClosestActor))
	{
		ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass()); // link to the nearest actor
	}

	CachedTurret->CurrentTarget = ClosestActor;
	CachedTurret->OnCurrentTargetSet();
}

void AAITurretController::DeathEvent()
{
	CachedTurret->DeathEvent();
}

void AAITurretController::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CachedTurret->IsAlive())
	{
		return;
	}

	CachedTurret->TakeDamageTurret(Damage);

	if (CachedTurret->GetHealth() <= 0.0f)
	{
		DeathEvent();
		//CachedTurret->Destroy();
		return;
	}

	const FVector TargetLocation = DamageCauser->GetActorLocation();
	const FVector TurretLocation = CachedTurret->GetActorLocation();
	if ((TargetLocation - TurretLocation).SizeSquared() <= FMath::Square(MaxFiringDistanceRadius))
	{
		CachedTurret->CurrentTarget = DamageCauser;
		CachedTurret->OnCurrentTargetSet();
	}
}
