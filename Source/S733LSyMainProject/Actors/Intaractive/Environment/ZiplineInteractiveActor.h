#pragma once

#include "CoreMinimal.h"
#include "Actors/Intaractive/InteractiveActor.h"
#include "ZiplineInteractiveActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API AZiplineInteractiveActor : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AZiplineInteractiveActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	bool GetIsOnZipline() const { return bIsOnZipline; }

	FRotator GetCableRotation();

	UCapsuleComponent* GetZiplineInteractionCapsule() const;

	FVector GetTopPointOfEndPillar() const;
	FVector GetTopPointOfStartPillar() const;
	FVector GetZiplineAnimationOffset() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float StartPillarHeight = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float EndPillarHeight = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float CableLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float InteractionCapsuleRadius = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector StartPillarLocation = FVector(0.0f, -CableLength * 0.5f, StartPillarHeight * 0.5f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector EndPillarLocation = FVector(0.0f, CableLength * 0.5f, EndPillarHeight * 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector StartPillarTopPoint = FVector(StartPillarLocation.X, StartPillarLocation.Y, StartPillarLocation.Z + StartPillarHeight * 0.5f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector EndPillarTopPoint = FVector(EndPillarLocation.X, EndPillarLocation.Y, EndPillarLocation.Z + EndPillarHeight * 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector ZiplineAnimationOffset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StartPillarMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* EndPillarMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CableMesh;

private:
	bool bIsOnZipline = false;

	FVector StartPillarTop;

	FVector EndPillarTop;
};
