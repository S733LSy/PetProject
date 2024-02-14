
#include "Door.h"

#include "S733LSyMainProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"

ADoor::ADoor()
{
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DefaultSceneRoot);

	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(GetRootComponent());

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorPivot);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // чтобы дверь просто так не делала этот тик
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(DoorAnimationCurve))
	{
		FOnTimelineFloatStatic DoorAnimationDelegate;
		DoorAnimationDelegate.BindUObject(this, &ADoor::UpdateDoorAnimation);
		DoorOpenTimeline.AddInterpFloat(DoorAnimationCurve, DoorAnimationDelegate);

		FOnTimelineEventStatic DoorOpenedDelegate;
		DoorOpenedDelegate.BindUObject(this, &ADoor::OnDoorAnimationFinished);
		DoorOpenTimeline.SetTimelineFinishedFunc(DoorOpenedDelegate);
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DoorOpenTimeline.TickTimeline(DeltaTime);
}

void ADoor::Interact(ASMBaseCharacter* Character)
{
	ensureMsgf(IsValid(DoorAnimationCurve), TEXT("Door animation curve is not set"));
	InteractWithDoor();

	if (OnInteractionEvent.IsBound())
	{
		OnInteractionEvent.Broadcast();
	}
}

FName ADoor::GetActionEventName() const
{
	return ActionInteract;
}

bool ADoor::HasOnInteractionCallback() const
{
	return true;
}

FDelegateHandle ADoor::AddOnInteractionUFunction(UObject* Object, const FName& FunctionName)
{
	return OnInteractionEvent.AddUFunction(Object, FunctionName);
}

void ADoor::RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle)
{
	OnInteractionEvent.Remove(DelegateHandle);
}

void ADoor::OnLevelDeserialized_Implementation()
{
	float YawAngle = bIsOpened ? AngleOpened : AngleClosed;
	DoorPivot->SetRelativeRotation(FRotator(0.0f, YawAngle, 0.0f));
}

void ADoor::InteractWithDoor()
{
	SetActorTickEnabled(true);
	if (bIsOpened)
	{
		DoorOpenTimeline.Reverse();
	}
	else
	{
		DoorOpenTimeline.Play();
	}
	bIsOpened = !bIsOpened;
}

void ADoor::UpdateDoorAnimation(float Alpha)
{
	float YawAngle = FMath::Lerp(AngleClosed, AngleOpened, FMath::Clamp(Alpha, 0.0f, 1.0f));
	DoorPivot->SetRelativeRotation(FRotator(0.0f, YawAngle, 0.0f));
}

void ADoor::OnDoorAnimationFinished()
{
	SetActorTickEnabled(false);
}

