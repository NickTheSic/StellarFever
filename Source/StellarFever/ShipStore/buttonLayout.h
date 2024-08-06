// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "buttonLayout.generated.h"

/**
 *
 */
UCLASS()
class STELLARFEVER_API UbuttonLayout : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetPurchaseIndex(uint32 index);
	void SetCostText(FString string);
	void SetValueText(FString string);
	void SetNameText(FString string);
	void SetStoreManager(class AStoreManager* storemanager);
	void SetStoreMenu(class UStoreMenu* menu);


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* pItem = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* pCost = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* pValue = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UShipStoreButton* ClickButton = nullptr;


private:

	class UStoreMenuWidget* pStoreWidget = nullptr;
	class AStoreManager* pStoreManager = nullptr;
	class UStoreMenu* pMenu = nullptr;
	uint32 uiIndex;
};
