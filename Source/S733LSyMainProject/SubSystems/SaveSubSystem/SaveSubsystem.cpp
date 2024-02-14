// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSubsystem.h"

#include "SaveSubsystemInterface.h"
#include "SaveSubSystemTypes.h"
#include "SaveSubsystemUtils.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"

const FGameSaveData& USaveSubsystem::GetGameSaveData() const
{
	return GameSaveData;
}

void USaveSubsystem::SaveGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SaveGame(): %s"), *GetNameSafe(this));

	SerializeGame(); // сериализация игры
	WriteSaveToFile(); // запись в файл
}

void USaveSubsystem::LoadLastGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s"), *GetNameSafe(this));

	if (SaveIds.Num() == 0) // проверяет есть ли сохранения по айди
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s Failed! No saves."), *GetNameSafe(this));
		return;
	}

	LoadGame(SaveIds[SaveIds.Num() - 1]); // загрузка игры с последним идентификатором, который у нас есть
}

void USaveSubsystem::LoadGame(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadGame()"));
	if (!SaveIds.Contains(SaveId)) // проверяет, что массив идентификаторов содержит сейв айди
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadGame(): Failed!"));
		return;
	}

	LoadSaveFromFile(SaveId); // загрузка из файла
	UGameplayStatics::OpenLevel(this, GameSaveData.LevelName); // открывает уровень
}

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Initialize(): %s"), *GetNameSafe(this));

	GameSaveData = FGameSaveData(); // создаём GameSaveData по умолчанию

	SaveDirectoryName = FString::Printf(TEXT("%sSaveGames/"), *FPaths::ProjectSavedDir()); // записываем адрес директории
	// куда будет сохранена игры, папка sSaveGames в *FPaths::ProjectSavedDir() папка сохранения проекта

	FSaveDirectoryVisitor DirectoryVisitor(SaveIds); // создаём FSaveDirectoryVisitor
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*SaveDirectoryName, DirectoryVisitor); // итерируемся по нашей директории,
	//чтобы заполнить SaveId
	SaveIds.Sort(); // считываем айди и сортируем

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USaveSubsystem::OnPostLoadMapWithWorld); // подписываем на глобальный делегат метод, после того,
	// как карта загрузилась
}

void USaveSubsystem::Deinitialize()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Deinitialize(): %s"), *GetNameSafe(this));

	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this); // отписываемся от делегата

	Super::Deinitialize();
}

void USaveSubsystem::SerializeLevel(const ULevel* Level, const ULevelStreaming* StreamingLevel)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeLevel(): %s, Level: %s, StreamingLevel: %s"), *GetNameSafe(this), *GetNameSafe(Level), *GetNameSafe(StreamingLevel));

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level; // считываем LevelSaveData из &GameSaveData.Level, который мы создали в методе initialize

	TArray<FActorSaveData>& ActorsSaveData = LevelSaveData->ActorsSaveData; // копируем ActorsSaveData
	ActorsSaveData.Empty(); // 

	for (AActor* Actor : Level->Actors) // итерируемся по всем экторам на уровне
	{
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>())
		{
			continue;
		}

		FActorSaveData& ActorSaveData = ActorsSaveData[ActorsSaveData.AddUnique(FActorSaveData(Actor))]; // создаём новую информацию об экторе
		ActorSaveData.Transform = Actor->GetTransform(); // получаем трансформ эктора, то есть местоположение и ориентация
		TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData.ComponentsSaveData;
		ComponentsSaveData.Empty();
		for (UActorComponent* ActorComponent : Actor->GetComponents())
		{
			if (ActorComponent->Implements<USaveSubsystemInterface>())
			{
				FObjectSaveData& ComponentSaveData = ComponentsSaveData[ComponentsSaveData.Emplace(ActorComponent)];
				FMemoryWriter MemoryWriter(ComponentSaveData.RawData, true);
				FSaveSubsystemArchive Archive(MemoryWriter, false);
				ActorComponent->Serialize(Archive);
			}
		}

		FMemoryWriter MemoryWriter(ActorSaveData.RawData, true); // создаём MemoryWriter от специального класса для записи
		//(в какой сегмент памяти записываем, что мы используем для сохранения байтовой информации)
		FSaveSubsystemArchive Archive(MemoryWriter, false); // создаём архив, куда и передаём MemoryWriter
		Actor->Serialize(Archive); // сериализация эктора
		// за счёт того, что архив создан от FMemoryWriter как раз и будет реализовываться запись в данный архив, а не чтение из него
	}
}

void USaveSubsystem::DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Level: %s, StreamingLevel: %s"), *GetNameSafe(this), *GetNameSafe(Level), *GetNameSafe(StreamingLevel));

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level; // считываем информацию о сохраненном уровне

	if (LevelSaveData == nullptr) // если нет никаких данных для этого уровня, то
	{
		// There is no save for this level yet. Call OnLevelDeserialized() in all cases!
		USaveSubsystemUtils::BroadcastOnLevelDeserialized(Level); // уровень ранее не был сохранен
		return;
	}

	TArray<AActor*> ActorsToNotify; // сохдаём пустой массив для экторов, которых должны оповестить и вызвать у них OnLevelDeserialized

	TArray<FActorSaveData*> ActorsSaveData; // создаём массив сохраненных данных
	ActorsSaveData.Reserve(LevelSaveData->ActorsSaveData.Num()); // зарезервируем его под наше количество сохраненных данных
	for (FActorSaveData& ActorSaveData : LevelSaveData->ActorsSaveData) 
	{
		ActorsSaveData.Add(&ActorSaveData); // поэлементно добавляем в массив все элементы из &ActorSaveData, которые у нас сейчас есть
	}

	for (TArray<AActor*>::TIterator ActorIterator = Level->Actors.CreateIterator(); ActorIterator; ++ActorIterator) // итерируемся по всем экторам с помощью итератора CreateIterator()
	{
		AActor* Actor = *ActorIterator; 
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>()) // если наш эктор не реализует сохранения, то мы переходим на следующую итерациб цикла
		{
			continue;
		}

		FActorSaveData* ActorSaveData = nullptr;
		for (TArray<FActorSaveData*>::TIterator ActorSaveDataIterator = ActorsSaveData.CreateIterator(); ActorSaveDataIterator; ++ActorSaveDataIterator) // находим данные для сохранения эктора
		{
			if ((*ActorSaveDataIterator)->Name == Actor->GetFName())
			{
				ActorSaveData = *ActorSaveDataIterator; 
				ActorSaveDataIterator.RemoveCurrent(); // нашли ActorSaveData, убираем для того, чтобы потом просто не итерироваться, чтобы в массиве остались только те экторы, которых нет на уровне
				// чтобы их потом добавить
				break;
			}
		}

		if (ActorSaveData == nullptr) // если данные пустые, то мы ничего не нашли для нашего эктора, значит он не был сохранен
		{
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, ActorSaveData not found! Destroy actor: %s"), *GetNameSafe(this), *GetNameSafe(Actor));
			Actor->Destroy(); // уничтожаем эктор
		}
		else
		{
			DeserializeActor(Actor, ActorSaveData); // считываем данные на наш эктор

			if (Actor->Implements<USaveSubsystemInterface>()) // если эктор реализует USaveSubsystemInterface
			{
				ActorsToNotify.Add(Actor); // то добавляем в массив экторов, которые должны оповестить добавляем наш эктор
			}
		}
	}

	UWorld* const World = GetWorld();

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.OverrideLevel = Level;
	ActorSpawnParameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (FActorSaveData* ActorSaveData : ActorsSaveData) // обходим экторов
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());

		ActorSpawnParameters.Name = ActorSaveData->Name; // в ActorSpawnParameters добавляем имя эктора

		// @bIgnoreOnActorSpawnedCallbackFast is hook to avoid double @OnLevelDeserialized invocation!
		BoolScopeWrapper OnActorSpawnedHook(bIgnoreOnActorSpawnedCallback, true); // после выхода из цикла он выставится фолс

		AActor* Actor = World->SpawnActor(ActorSaveData->Class.Get(), &ActorSaveData->Transform, ActorSpawnParameters); // создаём новый эктор

		if (!IsValid(Actor))
		{
			// This is valid case. Actor can be IsPendingKill due some actor's logic started from overlaps.
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Failed to spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());
			continue;
		}

		// Actor name can change so update it.
		ActorSaveData->Name = Actor->GetFName(); // записываем в данные для сохранения имя нашего эктора, потому что имя в процессе создания эктора может обновиться

		DeserializeActor(Actor, ActorSaveData); // считываем информацию о нашем экторе

		if (Actor->Implements<USaveSubsystemInterface>())
		{
			ActorsToNotify.Add(Actor); // добавляем в список для нотификации
		}
	}

	for (AActor* Actor : ActorsToNotify) // проходимся по всем экторам, которые были добавлены и
	{
		NotifyActorsAndComponents(Actor); // вызываем метод NotifyActorsAndComponents
	}
}

void USaveSubsystem::NotifyActorsAndComponents(AActor* Actor)
{
	ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (ActorComponent->Implements<USaveSubsystemInterface>())
		{
			ISaveSubsystemInterface::Execute_OnLevelDeserialized(ActorComponent);
		}
	}
}

void USaveSubsystem::SerializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeGame(): %s"), *GetNameSafe(this));

	UGameInstance* GameInstance = GetGameInstance(); // получаем гейминстанс
	GameSaveData.GameInstance = FObjectSaveData(GetGameInstance()); // из гейминстанса сохраняем информацию в GameSaveData
	FMemoryWriter MemoryWriter(GameSaveData.GameInstance.RawData, true); // создаём MemoryWriter
	FSaveSubsystemArchive Archive(MemoryWriter, false); // создаём архив
	GameInstance->Serialize(Archive); // сериализуем, записываем

	const UWorld* World = GetWorld(); // получаем мир
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld()); // получаем имя мира
	if (World->IsPlayInEditor())
	{
		LevelName = UWorld::RemovePIEPrefix(LevelName);
	}

	GameSaveData.LevelName = FName(LevelName); // записываем имя, которое получили в данные

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0); // получаем старт трансформ для базового персонажа
	// только для одного персонажа на нашей машине
	if (IsValid(PlayerCharacter))
	{
		GameSaveData.StartTransform = PlayerCharacter->GetTransform(); // в данные записываем этот трансформ
	}

	SerializeLevel(World->PersistentLevel); // сериализуем уровень
}

void USaveSubsystem::DeserializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeGame(): %s"), *GetNameSafe(this));

	UGameInstance* GameInstance = GetGameInstance(); // получаем гейминстанс
	FMemoryReader MemoryReader(GameSaveData.GameInstance.RawData, true); // создаём MemoryReader читатель
	FSaveSubsystemArchive Archive(MemoryReader, false); // создаём архив
	GameInstance->Serialize(Archive); // десериализуем, считываем данные

	const UWorld* World = GetWorld(); // получаем мир

	DeserializeLevel(World->PersistentLevel); // десериализуем уровень, читаем всю информацию
}

void USaveSubsystem::WriteSaveToFile()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::WriteSaveToFile(): %s"), *GetNameSafe(this));

	const int32 SaveId = GetNextSaveId(); // получаем след сейв айди
	SaveIds.AddUnique(SaveId); // добавляем в массив айдишек

	GameSaveData.Name = FName(FString::FromInt(SaveId)); // получаем имя GameSaveData, по сути имя нашего айдишника

	TArray<uint8> SaveBytes; // создаём массив для записи бинарной информации
	FMemoryWriter MemoryWriter(SaveBytes); // создаём MemoryWriter для записи
	FObjectAndNameAsStringProxyArchive WriterArchive(MemoryWriter, false); // создаём архив
	GameSaveData.Serialize(WriterArchive); // сериализуем в данный архив

	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*GetSaveFilePath(SaveId)); // создаём FileWriter (структура данных для записи файлов)

	if (bUseCompressedSaves) // если используется система сжатия сохранений, то
	{
		TArray<uint8> CompressedSaveBytes;
		FArchiveSaveCompressedProxy CompressedArchive(CompressedSaveBytes, NAME_Zlib); // создаётся компрессед архив
		CompressedArchive << SaveBytes; // записываем в него байты
		CompressedArchive.Flush(); // метод сжатия (делает сжатие)

		*FileWriter << CompressedSaveBytes; // в FileWriter передаём CompressedSaveBytes
	}
	else // если не используется система сжатия сохранений, то 
	{
		*FileWriter << SaveBytes; // в FileWriter записываются наши байты, которые заполнили с помощью метода Serialize
	}

	FileWriter->Close(); // сохраняем файл на диск
	delete FileWriter; // удаляем саму эту структуру
}

void USaveSubsystem::LoadSaveFromFile(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadSaveFromFile(): %s, SaveId %i"), *GetNameSafe(this), SaveId);

	FArchive* FileReader = IFileManager::Get().CreateFileReader(*GetSaveFilePath(SaveId)); // создаём чтеца
	TArray<uint8> SaveBytes; // массив сохраненных байт

	if (bUseCompressedSaves)
	{
		TArray<uint8> CompressedSaveBytes;
		*FileReader << CompressedSaveBytes;

		FArchiveLoadCompressedProxy DecompressedArchive(CompressedSaveBytes, NAME_Zlib); // разархивируем байты
		DecompressedArchive << SaveBytes; // считываем байты
		DecompressedArchive.Flush();
	}
	else
	{
		*FileReader << SaveBytes;
	}

	FileReader->Close(); // закрываем
	delete FileReader;

	FMemoryReader MemoryReader(SaveBytes, true); // создаём читателя
	FObjectAndNameAsStringProxyArchive ReaderArchive(MemoryReader, true); // создаём новый архив
	GameSaveData = FGameSaveData(); // создайм новый FGameSaveData
	GameSaveData.Serialize(ReaderArchive); // считываем информацию из нашего архива
}

void USaveSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnPostLoadMapWithWorld(): %s, World: %s"), *GetNameSafe(this), *GetNameSafe(LoadedWorld));
	DeserializeGame(); // десериализация игры, ту информацию, которую прочитали записываем в нашу игру
}

void USaveSubsystem::DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeActor(): %s, Actor %s"), *GetNameSafe(this), *GetNameSafe(Actor));

	Actor->SetActorTransform(ActorSaveData->Transform); // выставляем трансформу, считываем ActorSaveData->Transform
	
	const TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData->ComponentsSaveData;
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (ActorComponent->Implements<USaveSubsystemInterface>())
		{
			const FObjectSaveData* ComponentSaveData = ComponentsSaveData.FindByPredicate([=](const FObjectSaveData& SaveData) { return SaveData.Name == ActorComponent->GetFName();} );
			FMemoryReader MemoryReader(ComponentSaveData->RawData, true);
			FSaveSubsystemArchive Archive(MemoryReader, false);
			ActorComponent->Serialize(Archive);
		}
	}

	FMemoryReader MemoryReader(ActorSaveData->RawData, true); // создаём читателя, RawData - бинарное представление
	FSaveSubsystemArchive Archive(MemoryReader, false); // создаём архив
	Actor->Serialize(Archive); // записывает в архив
}

FString USaveSubsystem::GetSaveFilePath(int32 SaveId) const
{
	return SaveDirectoryName / FString::Printf(TEXT("%i.save"), SaveId);
}

int32 USaveSubsystem::GetNextSaveId() const
{
	if (SaveIds.Num() == 0)
	{
		return 1;
	}

	return SaveIds[SaveIds.Num() - 1] + 1;
}

void USaveSubsystem::OnActorSpawned(AActor* SpawnedActor)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s, Actor %s"), *GetNameSafe(this), *GetNameSafe(SpawnedActor));
	if (bIgnoreOnActorSpawnedCallback) // если выставлен флаг инорирования коллбэка, то мы ничего не делаем
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s, Skipped for actor %s. bIgnoreOnActorSpawnedCallback == true!"), *GetNameSafe(this), *GetNameSafe(SpawnedActor));
		return;
	}

	if (IsValid(SpawnedActor) && SpawnedActor->Implements<USaveSubsystemInterface>()) // если валиден эктор и реализует USaveSubsystemInterface
	{
		// We should notify a runtime spawned actors too.
		NotifyActorsAndComponents(SpawnedActor);
	}
}
