
#include "ZiplineInteractiveActor.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/KismetArrayLibrary.h"
#include "S733LSyMainProject/S733LSyMainProjectTypes.h"
#include "Components/SplineComponent.h"

AZiplineInteractiveActor::AZiplineInteractiveActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	StartPillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HigherPost"));
	StartPillarMesh->SetCollisionProfileName(CollisionProfilePawn);
	StartPillarMesh->SetupAttachment(RootComponent);

	EndPillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LowerPost"));
	EndPillarMesh->SetCollisionProfileName(CollisionProfilePawn);
	EndPillarMesh->SetupAttachment(RootComponent);

	CableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	CableMesh->SetupAttachment(RootComponent);
	CableMesh->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractiveVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZiplineInteractiveActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	StartPillarMesh->SetRelativeLocation(StartPillarLocation + StartPillarHeight * 0.5 * FVector::UpVector);
	EndPillarMesh->SetRelativeLocation(EndPillarLocation + EndPillarHeight * 0.5 * FVector::UpVector);


	UStaticMesh* StartPillar = StartPillarMesh->GetStaticMesh();
	if (IsValid(StartPillar))
	{
		float MeshHeight = StartPillar->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			StartPillarMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, StartPillarHeight / MeshHeight));
		}
	}
	UStaticMesh* EndPillar = EndPillarMesh->GetStaticMesh();
	if (IsValid(EndPillar))
	{
		float MeshHeight = EndPillar->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			EndPillarMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, EndPillarHeight / MeshHeight));
		}
	}
	StartPillarTop = StartPillarLocation + FVector::UpVector * StartPillarHeight;
	EndPillarTop = EndPillarLocation + FVector::UpVector * EndPillarHeight;
	UStaticMesh* Cable = CableMesh->GetStaticMesh();
	if (IsValid(Cable))
	{
		float MeshLenght = Cable->GetBoundingBox().GetSize().X;
		if (!FMath::IsNearlyZero(MeshLenght))
		{
			CableLength = (StartPillarTop - EndPillarTop).Size();
			CableMesh->SetRelativeScale3D(FVector(CableLength / MeshLenght, 1.0, 1.0f));
			UCapsuleComponent* InteractionCapsuleVolume = GetZiplineInteractionCapsule();
			InteractionCapsuleVolume->SetCapsuleHalfHeight(CableLength * 0.5);
			InteractionCapsuleVolume->SetCapsuleRadius(InteractionCapsuleRadius);
		}
	}
	FVector CableLocation = (StartPillarTop + EndPillarTop) * 0.5;
	CableMesh->SetRelativeLocation(CableLocation);
	FRotator CableRotation = CableMesh->GetRelativeRotation();
	CableRotation = (EndPillarTop - StartPillarTop).ToOrientationRotator();
	CableMesh->SetRelativeRotation(CableRotation);
	CableRotation.Pitch -= 90;
	InteractionVolume->SetRelativeRotation(CableRotation);
	InteractionVolume->SetRelativeLocation(CableLocation);
}

void AZiplineInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AZiplineInteractiveActor::OnInteractionVolumeOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AZiplineInteractiveActor::OnInteractionVolumeOverlapEnd);
}

FRotator AZiplineInteractiveActor::GetCableRotation()
{
	return CableMesh->GetRelativeRotation();
}

UCapsuleComponent* AZiplineInteractiveActor::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

FVector AZiplineInteractiveActor::GetTopPointOfEndPillar() const
{
	FVector EndTop = (EndPillarMesh->GetComponentLocation() + EndPillarHeight * 0.5f * FVector::UpVector);
	return EndTop;
}

FVector AZiplineInteractiveActor::GetTopPointOfStartPillar() const
{
	FVector StartTop = (StartPillarMesh->GetComponentLocation() + StartPillarHeight * 0.5f * FVector::UpVector);
	return StartTop;
}

FVector AZiplineInteractiveActor::GetZiplineAnimationOffset() const
{
	return ZiplineAnimationOffset;
}