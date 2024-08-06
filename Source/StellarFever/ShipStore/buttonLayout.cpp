// Copyright Quarantine Games 2020


#include "buttonLayout.h"
#include "StoreMenuWidget.h"
#include "Components/Button.h"
#include "ShipStoreButton.h"
#include "Components/TextBlock.h"
#include "StoreManager.h"
#include "StoreMenu.h"


void UbuttonLayout::SetPurchaseIndex(uint32 index)
{
	uiIndex = index;
	ClickButton->SetPurchaseIndex(uiIndex);
}

void UbuttonLayout::SetCostText(FString string)
{
	pCost->SetText(FText::FromString(string));
}

void UbuttonLayout::SetValueText(FString string)
{
	pValue->SetText(FText::FromString(string));
}

void UbuttonLayout::SetNameText(FString string)
{
	pItem->SetText(FText::FromString(string));

}

void UbuttonLayout::SetStoreManager(AStoreManager* storemanager)
{
	if (storemanager != nullptr)
	{
		pStoreManager = storemanager;
		ClickButton->SetStoreManager(pStoreManager);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Setting the player for the Purchase button was a null player"));
}

void UbuttonLayout::SetStoreMenu(UStoreMenu* menu)
{
	pMenu = menu;
	ClickButton->SetStoreMenu(pMenu);
}
