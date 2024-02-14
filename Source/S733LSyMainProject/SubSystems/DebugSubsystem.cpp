// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugSubsystem.h"

bool UDebugSubsystem::IsCategoryEnabled(const FName& CategoryName) const
{
	const bool* bIsEnabled = EnabledDebugCategories.Find(CategoryName); // возвращает указатель на значение
	return bIsEnabled != nullptr && *bIsEnabled; // *bIsEnabled - получение значения по адресу указателя
}

void UDebugSubsystem::EnableDebugCategory(const FName& CategoryName, bool bIsEnabled)
{
	EnabledDebugCategories.FindOrAdd(CategoryName);
	EnabledDebugCategories[CategoryName] = bIsEnabled;
}
