
#pragma once

#include "CoreMinimal.h"
#include "Actors/Intaractive/InteractiveActor.h"
#include "Ladder.generated.h"

class UAnimMontage;
class UBoxComponent;
class UStaticMeshComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class S733LSYMAINPROJECT_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()

public:
	ALadder();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	float GetLadderHeight() const;

	bool GetIsOnTop() const { return bIsOnTop; }

	UAnimMontage* GetAttachFromTopAnimMontage() const;

	FVector GetAttachFromTopAnimMontageStartingLocation() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float LadderHeight = 100.0f; // высота лестницы
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float LadderWidth = 50.0f; // ширина лестницы
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float StepsInterval = 25.0f; // интревал между ступеньками
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float BottomStepOffset = 25.0f; // расстояние от пола до первой ступеньки

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* RightRailMeshComponent; // правый поручень лестницы
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LeftRailMeshComponent; // левый поручень лестницы
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UInstancedStaticMeshComponent* StepsMeshComponent; // сами ступеньки, у одного эктора можно создать много ступенек
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* TopInteractionVolume;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		UAnimMontage* AttachFromTopAnimMontage;
	// Offset from ladder's top for starting anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector; // Offset from ladder's top for starting anim montage

	UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	bool bIsOnTop = false;
};
