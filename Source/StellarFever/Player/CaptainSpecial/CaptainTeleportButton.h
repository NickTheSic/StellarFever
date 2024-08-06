// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "CaptainTeleportButton.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UCaptainTeleportButton : public UButton
{
	GENERATED_BODY()

public:
	UCaptainTeleportButton();

	void CreateTextSubWidget(UWidgetTree* widgetTree);

	UFUNCTION()
		void SetPlayerToTeleport();

	void SetCaptain(class ACaptainPlayer* captain);
	void SetCaptainScreen(class UCaptainScreen* screen);
	void SetButtonText(FString string);
	void SetTeleportIndex(uint32 idx); //force it to be idx >= 0

protected:
	class UTextBlock* pText = nullptr;
	class ACaptainPlayer* pCaptain = nullptr;
	class UCaptainScreen* pScreen = nullptr;

	int32 iTeleportIndex = -2; //This is the default failed/cancelled

};
