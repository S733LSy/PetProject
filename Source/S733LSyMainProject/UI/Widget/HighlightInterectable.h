// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HighlightInterectable.generated.h"

class UTextBlock;
UCLASS()
class S733LSYMAINPROJECT_API UHighlightInterectable : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetActionText(FName KeyName);

protected:
	UPROPERTY(meta = (BindWidget)) // ���� �� �������� ����� ������ � ����� �� ������ � �����, �� �� ������������� � ����� ��������
	UTextBlock* ActionText;
};
