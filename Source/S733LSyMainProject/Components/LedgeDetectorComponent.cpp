
#include "LedgeDetectorComponent.h"
#include "Utils/SMTraceUtils.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "S733LSyMainProjectTypes.h"
#include "Kismet/GameplayStatics.h"
#include "S733LSyMainProject/SMGameInstance.h"
#include "SubSystems/DebugSubsystem.h"
#include "MovementComponents/SMBaseCharacterMovementComponent.h"

void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectorComponent::BeginPlay() only a character can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
	DefaultCapsuleRadius = CachedCharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	DefaultCapsuleHalfHeight = CachedCharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
		UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();

		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = true; // для сложной коллизии
		QueryParams.AddIgnoredActor(GetOwner());

#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
#else
		bool bIsDebugEnabled = false;
#endif
		float DrawTime = 2.0f;

		float BottomZOffset = 2.0f; // небольшой подъем капсулы
		CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector; // получаем самую нижнюю точку персонажа

		//1. Forward check
		float ForwardCheckCapsuleRadius = DefaultCapsuleRadius;
		float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5f;

		FHitResult ForwardCheckHitResult; // чтобы получать значения
		//FCollisionShape ForwardCapsuleShape = FCollisionShape::MakeCapsule(ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight); // создание капсулы для проверки препятствия перед персонажем
		FVector ForwardStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;//позиция с которой начинается пространственный запрос (происходит умножение на (0.0f, 0.0f, 1.0f))
		FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

		if (!SMTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
		{
			return false;
		}

		//2. Donward check
		float DownwardSphereCheckRadius = DefaultCapsuleRadius;

		float DownwardCheckDepthOffset = 10.0f;
		FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
		DownwardStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardSphereCheckRadius;
		FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);

		if (!SMTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, DownwardSphereCheckRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
		{
			return false;
		}

		//3. Overlap check
		float OverlapCapsuleRadius = DefaultCapsuleRadius;
		float OverlapCapsuleHalfHeight = DefaultCapsuleHalfHeight;
		float OverlapCapsuleFloorOffset = 2.0f;
		FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;
		FVector ComponentHitLocation = ForwardCheckHitResult.GetComponent()->GetComponentLocation();

		if (SMTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bIsDebugEnabled, DrawTime))
		{
			return false;
		}

		//LedgeDescription.AbsLocation = OverlapLocation;
		LedgeDescription.Location = OverlapLocation; // -ComponentHitLocation; // точка на которую хотим забраться
		LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator(); // вращение на которое необходимо повернуть игрока // Компонентное произведение, компоненты которого перемножены на значения, то есть будет вектор (-x, -y, 0) от impactnormal
		LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;
		LedgeDescription.Component = DownwardCheckHitResult.GetComponent();
		LedgeDescription.AbsLocation = DownwardCheckHitResult.GetComponent()->GetComponentLocation();
		return true;
}

FVector ULedgeDetectorComponent::GetCharacterBottomLocation()
{
	return CharacterBottom;
}

FVector ULedgeDetectorComponent::GetDownwardCheckHitResult()
{
	return DownwardCheckHitResult.ImpactPoint;
}
