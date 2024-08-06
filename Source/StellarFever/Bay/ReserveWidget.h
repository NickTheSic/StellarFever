// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReserveWidget.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UReserveWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//TWeakPtr<AMedicBay> OwnerMedBay;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* ReserveBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentReserveLabel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MaxReserveLabel;
};
