// Copyright Quarantine Games 2020


#include "ShipStoreButton.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "StoreManager.h"
#include "StoreMenu.h"


UShipStoreButton::UShipStoreButton()
{
	OnClicked.AddDynamic(this, &UShipStoreButton::SetPlayerToPurchase);

	FSlateBrush hovered;
	hovered.TintColor = FSlateColor(FLinearColor::Blue);
	FButtonStyle RegularStyle;
	RegularStyle.SetHovered(hovered);
	SetStyle(RegularStyle);

	SetPressMethod(EButtonPressMethod::ButtonPress);
}

void UShipStoreButton::CreateTextSubWidget(UWidgetTree* widgetTree)
{
	pText = widgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	AddChild(pText);
}

void UShipStoreButton::SetPlayerToPurchase()
{
	if (pStoreManager != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "saying to purchase");
		pStoreManager->SetPurchaseInformation(iPurchaseIndex);
		pStoreManager->PurchaseAvailableItems();
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "The player was null");
}

void UShipStoreButton::SetStoreManager(AStoreManager* storemanager)
{
	if (storemanager != nullptr)
	{
		pStoreManager = storemanager;
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Setting the player for the Purchase button was a null player"));
}

void UShipStoreButton::SetStoreMenu(UStoreMenu* menu)
{
	pMenu = menu;
}

void UShipStoreButton::SetButtonText(FString string)
{
	if (pText != nullptr)
	{
		pText->SetText(FText::FromString(string));

		FSlateColor slateColor(FLinearColor::Black);
		pText->SetColorAndOpacity(slateColor);

		FSlateFontInfo slateFontInfo = pText->Font;
		slateFontInfo.Size = 12;
		pText->SetFont(slateFontInfo);
	}
}

void UShipStoreButton::SetPurchaseIndex(uint32 idx)
{
	iPurchaseIndex = idx;
}
