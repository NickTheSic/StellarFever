// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ShipStoreButton.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UShipStoreButton : public UButton
{
	GENERATED_BODY()

public:
	UShipStoreButton();

	void CreateTextSubWidget(UWidgetTree* widgetTree);

	UFUNCTION()
		void SetPlayerToPurchase();

	void SetStoreManager(class AStoreManager* storemanager);
	void SetStoreMenu(class UStoreMenu* menu);
	void SetButtonText(FString string);
	void SetPurchaseIndex(uint32 idx); //force it to be idx >= 0

protected:
	class UTextBlock* pText = nullptr;
	class AStoreManager* pStoreManager = nullptr;
	class UStoreMenu* pMenu = nullptr;

	int32 iPurchaseIndex = -2; //This is the default failed/cancelled
	
};
