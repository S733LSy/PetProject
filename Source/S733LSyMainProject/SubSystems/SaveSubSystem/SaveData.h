// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "SaveData.generated.h"

USTRUCT()
struct FBaseSaveData
{
	GENERATED_BODY()

public:
	FBaseSaveData();

	virtual ~FBaseSaveData() {}
	virtual bool Serialize(FArchive& Archive); // если есть определенный класс, который лежит в некотором формате памяти и если захотим положить его в архив, то мы сериализуем данный класс
	// десиарилизация - чтение из архива

	// чтение или запись определяется типом архива (FArchive& Archive)
	friend FArchive& operator << (FArchive& Archive, FBaseSaveData& SaveData) // перегруженный оператор побитового сдвига влево
	{
		SaveData.Serialize(Archive);
		return Archive;
	}

	FName Name; // имя данных структур, чтобы различать разные данные для экторов, объектов и тд, чтобы можно было их сравнивать
};

FORCEINLINE bool operator == (const FBaseSaveData& First, const FBaseSaveData& Second) { return First.Name == Second.Name; } // оператор сравнения двух различных данных сохранения, данные являются одними и теми же,
// если у них одинаковое имя

/**
 * Object data that will be saved/loaded.
 */
USTRUCT()
struct FObjectSaveData : public FBaseSaveData // данные для сохранения любого объекта
{
	GENERATED_BODY()

public:
	FObjectSaveData(); // конструктор
	FObjectSaveData(const UObject* Object); // конструктор, который принимает uobject

	virtual bool Serialize(FArchive& Archive) override;

	TStrongObjectPtr<UClass> Class;
	TArray<uint8> RawData; // то как будут лежать данные, массив байтов, в которые записывается информация и читается
	// процесс перевода данных в массив байтов и является сериализацией
};

/**
 * Actor data that will be saved/loaded.
 */
USTRUCT()
struct FActorSaveData : public FObjectSaveData // информация о сохранении эктора
{
	GENERATED_BODY()

	FActorSaveData();
	FActorSaveData(const AActor* Actor);

	virtual bool Serialize(FArchive& Archive) override;

	TArray<FObjectSaveData> ComponentsSaveData;
	FTransform Transform; // будет сохраняться трансорма эктора, чтобы сохранять метоположение различных передвигаемых объектов
};

/**
 * Level data that will be saved/loaded.
 */
USTRUCT()
struct FLevelSaveData : public FBaseSaveData // данные для сохранения уровня
{
	GENERATED_BODY()

public:
	FLevelSaveData();
	FLevelSaveData(const FName& LevelName);

	virtual bool Serialize(FArchive& Archive) override;

	TArray<FActorSaveData> ActorsSaveData; // массив на сохраненных экторов
};

/**
 * Global game data that will be saved/loaded.
 */
USTRUCT()
struct FGameSaveData : public FBaseSaveData // самая важная структура, информация о сохранении игры
{
	GENERATED_BODY()

public:
	FGameSaveData();

	virtual bool Serialize(FArchive& Archive) override;

	FName LevelName; // имя уровня
	FLevelSaveData Level; // данные сохранения на уровень
	FObjectSaveData GameInstance; // данные сохранения о гейминстансе
	FTransform StartTransform; // начальное местоположение
};