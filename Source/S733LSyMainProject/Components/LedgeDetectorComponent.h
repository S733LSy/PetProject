// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectorComponent.generated.h"


USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladge description")
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladge description")
	FVector LedgeNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladge description")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladge description")
	FVector AbsLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladge description")
	UPrimitiveComponent* Component;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S733LSYMAINPROJECT_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);
	virtual FVector GetCharacterBottomLocation();
	virtual FVector GetDownwardCheckHitResult();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MinimumLedgeHeight = 40.0f; // Минимальная высота выступа на которую может забраться персонаж используя climbing

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MaximumLedgeHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float ForwardCheckDistance = 100.0f;

private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
	float DefaultCapsuleRadius;
	float DefaultCapsuleHalfHeight;
	FVector CharacterBottom;
	FHitResult DownwardCheckHitResult;
};
