
#include "SaveSubsystemUtils.h"
#include "SaveSubsystemInterface.h"
#include "SaveSubSystemTypes.h"

void USaveSubsystemUtils::BroadcastOnLevelDeserialized(ULevel* Level)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystemUtils::BroadcastOnLevelDeserialized(): %s"), *GetNameSafe(Level));
	if (!IsValid(Level))
	{
		return;
	}

	for (AActor* Actor : Level->Actors) // ������� ��� ������
	{
		if (IsValid(Actor) && Actor->Implements<USaveSubsystemInterface>()) // ���� ������� � ��������� ��������� USaveSubsystemInterface
		{
			ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
		}
	}
}
