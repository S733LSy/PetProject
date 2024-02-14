// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSaveSubsystem, Log, All);
const FName FileExtensionSave = TEXT("save");

/**
 * Used for loading and saving.
 */
// FObjectAndNameAsStringProxyArchive базовая структура для сериализации и хранения информации
struct FSaveSubsystemArchive : public FObjectAndNameAsStringProxyArchive // Архив который будет использоваться для сохранения и загрузки
{
	FSaveSubsystemArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails);
};

/**
 * Used to get save file names as save id's.
 */
class FSaveDirectoryVisitor : public IPlatformFile::FDirectoryVisitor // чтобы можно было входить в определенную папку, которая есть
{
public:
	FSaveDirectoryVisitor(TArray<int32>& InSaveIds);
	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override;

private:
	TArray<int32>& SaveIds;
};

/**
 * Used to change bool value for scope.
 */
struct BoolScopeWrapper // сохраняет старое значение и выставляет новое значение
{
public:
	BoolScopeWrapper(bool& bInValue, bool bNewValue);
	~BoolScopeWrapper();

private:
	bool& bValue;
	bool bInitialValue;
};