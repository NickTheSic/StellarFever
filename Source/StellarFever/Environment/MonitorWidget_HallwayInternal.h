// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MonitorWidget.h"
#include "MonitorWidget_HallwayInternal.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UMonitorWidget_HallwayInternal : public UMonitorWidget
{
	GENERATED_BODY()
protected:
		virtual void OnMonitorStateChanged_Internal(class AMonitorState* monitor, int32 prevID, int32 newID) override;

		UPROPERTY()
			FTimerHandle timer;

		UPROPERTY(meta = (BindWidget, OptionalWidget=true))
			class UBorder* Highlight_Greenhouse = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Airlock = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Bridge = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Engineering = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Depressure = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Armory = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Medbay = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Oxygen = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_LEngine = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_REngine = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UBorder* Highlight_Bathroom = nullptr;

		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UImage* Shield_Icon_ON = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UImage* Shield_Icon_OFF = nullptr;
		UPROPERTY(meta = (BindWidget, OptionalWidget = true))
			class UTextBlock* Shield_Text = nullptr;


		void NativeTick(const FGeometry& geo, float deltaTime) override;

		UFUNCTION()
			void UpdateStates();
};
