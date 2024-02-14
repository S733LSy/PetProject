
#include "CharacterAttributeComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "S733LSyMainProject/S733LSyMainProjectTypes.h"
#include "S733LSyMainProject/Characters/SMBaseCharacter.h"
#include "S733LSyMainProject/Components/MovementComponents/SMBaseCharacterMovementComponent.h"
#include "S733LSyMainProject/SubSystems/DebugSubsystem.h"


UCharacterAttributeComponent::UCharacterAttributeComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UCharacterAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAttributeComponent, Health);
}

float UCharacterAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float UCharacterAttributeComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

float UCharacterAttributeComponent::GetOxygenPercent() const
{
	return Oxygen / MaxOxygen;
}

void UCharacterAttributeComponent::AddHealth(float HealthToAdd)
{
	Health = FMath::Clamp(Health + HealthToAdd, 0.0f, MaxHealth);
	OnHealthChanged();
}

void UCharacterAttributeComponent::RestoreFullStamina()
{
	Stamina = MaxStamina;
}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(MaxHealth > 0.0f, TEXT("UCharacterAttributeComponent::BeginPlay() max health cannot be equal to 0"));
	checkf(GetOwner()->IsA<ASMBaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay UCharacterAttributeComponent can be used only with ASMBaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<ASMBaseCharacter*>(GetOwner());

	Health = MaxHealth;
	if (GetOwner()->HasAuthority()) // ������� �� ����� ����� ��������� ������ � �������
	{
		//�������� �� ������� �����
		CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);
	}

	Stamina = MaxStamina;
	Oxygen = MaxOxygen;
}

void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	const float DeltaStamina = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting() ? -SprintStaminaConsumptionVelocity : StaminaRestoreVelocity; // ���������� DeltaStamine �������� � ����������� �� ���� ����� �������� ��� ���
	Stamina = FMath::Clamp(Stamina + DeltaStamina * DeltaTime, 0.0f, MaxStamina);
	if (FMath::IsNearlyEqual(Stamina, MaxStamina))
	{
		OnStaminaUpdate.Broadcast();
		OutOfStaminaEvent.Broadcast(false);
	}
	else if (FMath::IsNearlyZero(Stamina))
	{
		OutOfStaminaEvent.Broadcast(true);
	}
}

void UCharacterAttributeComponent::UpdateOxygenValue(float DeltaTime)
{
	const float DeltaOxygen = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSwimming() ? -SwimOxygenConsumptionVelocity : OxygenRestoreVelocity; // ������������� DeltaOxygen �������� � ����������� �� ����, ����� �������� ��� ���
	Oxygen = FMath::Clamp(Oxygen + DeltaOxygen * DeltaTime, 0.0f, MaxOxygen); // ������ ������� �������� ��������� (�������������� ��� �����)
	if (FMath::IsNearlyEqual(Oxygen, MaxOxygen))
	{
		OnOxygenUpdateEvent.Broadcast();
	}
	if (FMath::IsNearlyEqual(Oxygen, 0.0f))
	{
		OutOfOxygen.Broadcast();
	}
}

void UCharacterAttributeComponent::OnRep_Health()
{
	OnHealthChanged();
}

void UCharacterAttributeComponent::OnHealthChanged()
{
	if (OnHealthChangedEvent.IsBound())
	{
		OnHealthChangedEvent.Broadcast(GetHealthPercent());
	}
	if (Health <= 0.0f) // �������� �� OnTakeAnyDamage
	{
		OnDeathEvent.Broadcast(); // ���������� ����, ��� ���������� �� ������ �������
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector HealthTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), HealthTextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);

	FVector StaminaTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 2.5f) * FVector::UpVector;
	DrawDebugString(GetWorld(), StaminaTextLocation, FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Blue, 0.0f, true);

	FVector OxygenTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), OxygenTextLocation, FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::Orange, 0.0f, true);
}
#endif

void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return; // ������� �� ������, ���� �������� ����
	}

	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s recevied %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth); // Clamp - ��������� �� ��������� Health - Damage ��������� � ��������� �� 0 �� ������������� ��������, ������ ��� �� �� ����� ���� ������� � ���� �������������
	OnHealthChanged();
}

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}
