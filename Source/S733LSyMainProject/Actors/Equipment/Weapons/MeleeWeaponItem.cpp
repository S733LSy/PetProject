
#include "MeleeWeaponItem.h"
#include "Characters/SMBaseCharacter.h"
#include "Components/Weapon/MeleeHitRegistrator.h"

AMeleeWeaponItem::AMeleeWeaponItem()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeaponItem::StartAttack(EMeleeAttackTypes AttackType)
{
	ASMBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	HitActors.Empty();

	// ����� ���� � ���� ���������� ������, ���� ��� ����
	CurrentAttack = Attacks.Find(AttackType); // Find ���������� ��������� �� ��������� ��������, � �� ���� ��������
	if (CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance))
		{
			float Duration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeaponItem::OnAttackTimerElapsed, Duration, false); // &AMeleeWeaponItem::OnAttackTimerElapsed ����������� �� ��������� �������
		}
		else
		{
			OnAttackTimerElapsed();
		}
	}
}

void AMeleeWeaponItem::SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled)
{
	HitActors.Empty(); // �������� �������
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators) // ����� ���������
	{
		HitRegistrator->SetIsHitRegistrationEnabled(bIsRegistrationEnabled);
	}
}

void AMeleeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistrator>(HitRegistrators); // �������� ��� ������������ ���������

	// ����� ��������� ������������� � �������� �� ������� ��������� �����, ������� ������ ProcessHit
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->OnMeleeHitRegistred.AddUFunction(this, FName("ProcessHit"));
	}

}

void AMeleeWeaponItem::ProcessHit(const FHitResult& HitResult, const FVector& HitDirection)
{
	if (CurrentAttack == nullptr)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor) || HitActor == GetOwner())
	{
		return;
	}

	if (HitActors.Contains(HitActor))
	{
		return;
	}

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = HitDirection;
	DamageEvent.DamageTypeClass = CurrentAttack->DamageTypeClass;

	ASMBaseCharacter* CharacterOwner = GetCharacterOwner();
	AController* Controller = IsValid(CharacterOwner) ? CharacterOwner->GetController<AController>() : nullptr;
	HitActor->TakeDamage(CurrentAttack->DamageAmount, DamageEvent, Controller, GetOwner());

	HitActors.Add(HitActor);
}

void AMeleeWeaponItem::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
	SetIsHitRegistrationEnabled(false);
}
