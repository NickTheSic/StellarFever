// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MonitorWidget.h"
#include "MonitorWidget_CargoInternal.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UMonitorWidget_CargoInternal : public UMonitorWidget
{
	GENERATED_BODY()
protected:
		virtual void OnMonitorStateChanged_Internal(class AMonitorState* monitor, int32 prevID, int32 newID) override;
	
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* VaccineCount = nullptr;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* LifeCount = nullptr;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* FuelCount = nullptr;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* GloobelCount = nullptr;

		UPROPERTY()
		FTimerHandle timer;

		//Updates the cargo monitor's information every second
		void UpdateInformation();
		virtual void NativeConstruct() override;
};
