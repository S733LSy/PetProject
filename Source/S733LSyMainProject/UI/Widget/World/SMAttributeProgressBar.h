// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SMAttributeProgressBar.generated.h"

class UProgressBar;
UCLASS()
class S733LSYMAINPROJECT_API USMAttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressPercantage(float Percentage);
	
protected:
	UPROPERTY(meta = (BindWidget)) // позвол€ет из C++ прив€затьс€ к блупринтовым виджетам
	UProgressBar* HealthProgressBar;
};
