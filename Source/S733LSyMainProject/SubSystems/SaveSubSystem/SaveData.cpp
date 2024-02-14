// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveData.h"

FBaseSaveData::FBaseSaveData()
{
}

bool FBaseSaveData::Serialize(FArchive& Archive)
{
	Archive << Name; // записывает данный архив имя файла, будет записывать, если архив на чтение, либо читать, если архив на чтение
	return true;
}

FObjectSaveData::FObjectSaveData()
	: Class(nullptr)
{
}

FObjectSaveData::FObjectSaveData(const UObject* Object)
	: Class(nullptr)
{
	if (IsValid(Object))
	{
		Name = Object->GetFName(); // получает имя объекта и записывает его в поле Name
		//таким образом мы можем понять с каким объектом работаем
		Class = TStrongObjectPtr<UClass>(Object->GetClass()); // получает класс данного объекта, чтобы записать в поле Class
	}
}

bool FObjectSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);

	UClass* ClassPtr = Class.Get(); // получает указатель на класс
	Archive << ClassPtr; // сериализует или десериализует класс из архива (<< как чтение так и запись)

	if (Archive.IsLoading() && IsValid(ClassPtr)) // если архив загружается
	{
		Class = TStrongObjectPtr<UClass>(ClassPtr); // записываем класс из указателя
	}

	Archive << RawData; // записываем в архив сырые данные, которые есть
	return true;
}

FActorSaveData::FActorSaveData()
	: Transform(FTransform::Identity)
{
}

FActorSaveData::FActorSaveData(const AActor* Actor)
	: Super(Actor) // принимает произвольный обджект и туда уже передасться информация об экторе, чтобы он сериализироваться в роу дэйта и добавиться в архив
	, Transform(FTransform::Identity)
{
}

bool FActorSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << Transform; // добавляем информацию о трансформе в архив
	Archive << ComponentsSaveData;
	return true;
}

FLevelSaveData::FLevelSaveData()
{
}

FLevelSaveData::FLevelSaveData(const FName& LevelName)
{
	Name = LevelName; // записывает имя
}

bool FLevelSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << ActorsSaveData; // записывает информацию о сохраненных экторах
	return true;
}

FGameSaveData::FGameSaveData()
	: Level(FName(TEXT("Persistent")))
	, StartTransform(FTransform::Identity)
{
}

bool FGameSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << LevelName; // сохраняет имя уровня
	Archive << Level; // сам уровень
	Archive << GameInstance; // гейминстанс
	Archive << StartTransform; // информация о точке начала игры
	return true;
}