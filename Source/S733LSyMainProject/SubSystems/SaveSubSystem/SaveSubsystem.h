// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

UCLASS()
class S733LSYMAINPROJECT_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	const FGameSaveData& GetGameSaveData() const; // геттер для FGameSaveData

	UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
	void SaveGame(); // для сохранения игры

	UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
	void LoadLastGame(); // для загрузки последней сохраненной игры

	UFUNCTION(BlueprintCallable, Category = "Save Subsystem")
	void LoadGame(int32 SaveId); // для загрузки с соответствующим идентификатором

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SerializeLevel(const ULevel* Level, const ULevelStreaming* StreamingLevel = nullptr); // метод сериализации уровня, записи
	void DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel = nullptr); // метод десериализации уровня, чтения
	
private:
	void SerializeGame(); // сериализация игры
	void DeserializeGame(); // десериализация игры
	void WriteSaveToFile(); // запись сохранения в файл
	void LoadSaveFromFile(int32 SaveId); // чтение сохранения из файла
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld); // метод, который вызывается после того, как карта загрузилась
	void DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData); // метод чтения эктора, для того, чтобы выставить его изначальную позицию
	FString GetSaveFilePath(int32 SaveId) const; // получение пути сохранения для определенного айди
	int32 GetNextSaveId() const; // получение следующего айди
	void OnActorSpawned(AActor* SpawnedActor); // привяжем к FDelegateHandle OnActorSpawnedDelegateHandle, когда какой-то эктор добавляется
	void NotifyActorsAndComponents(AActor* Actor);


	FGameSaveData GameSaveData; // самое главное поле - это данные сохранения, то место в памяти куда юудем сохранять инфу и потом считывать
	FString SaveDirectoryName; // имя директории, тобы понять где хранятся файлы
	TArray<int32> SaveIds; // массив идентификаторов наших сохранений
	FDelegateHandle OnActorSpawnedDelegateHandle; // делегат, который вызывается, когда какой-то эктор добавился в мир, это нужно для того, чтобы при необхоимости вызвать метод
	// сериализации и десериализации и обработать, чтобы всё было нормально

	bool bUseCompressedSaves = false; // сжатое сохранение или нет
	/** Used to avoid double @OnLevelDeserialized invocation */
	bool bIgnoreOnActorSpawnedCallback = false; // чтобы выключать OnLevelDeserialize методы на только что заспавненных экторах
};
