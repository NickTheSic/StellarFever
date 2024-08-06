// Copyright Quarantine Games 2020


#include "StoreMenuWidget.h"
#include "ShipStoreButton.h"
#include "StoreManager.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "StoreMenu.h"
#include "../ShipStore/StoreItem.h"
#include "../Miscellaneous/ShipState.h"
#include "buttonLayout.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"

UStoreMenuWidget::UStoreMenuWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	buttonTemplate = UShipStoreButton::StaticClass();
	horizontalTemplate = UHorizontalBox::StaticClass();
}
void UStoreMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (exitButton)
	{
		exitButton->OnClicked.AddDynamic(this, &UStoreMenuWidget::ExitMenu);
		exitButton->SetPressMethod(EButtonPressMethod::ButtonPress);
	}
}

void UStoreMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (pStoreManager->pshipstate)
	{
		int coin = pStoreManager->pshipstate->GetMoney();
		CurrencyText->SetText(FText::FromString(FString::FromInt(coin)));
	}
}

void UStoreMenuWidget::CreateButtons(int32 numButtons)
{
	if (numButtons == 0)
	{
		//Set the title text to say there isn't any places
		titleText->SetText(FText::FromString(TEXT("No Item Available")));
		FWidgetTransform trans;
		trans.Translation.Y = 58;
		trans.Translation.X = 30;
		titleText->Slot->Content->SetRenderTransform(trans);
		return;
	}

	if (buttonTemplate != nullptr)
	{
		UHorizontalBox* horizontal = nullptr;
		titleText->SetText(FText::FromString(TEXT("Welcome To Shop!! Please Buy!!")));

		FName temp = FName("HorizBox" + FString::FromInt(0));

		for (int i = 0; i < numButtons; i++)
		{
			/*if (i % 2 == 0)
			{
				horizontal = nullptr;
				int num = i / 2;
				FName name2 = FName("HorizBox" + FString::FromInt(num));
				horizontal = WidgetTree->ConstructWidget<UHorizontalBox>(horizontalTemplate, name2);
				buttonList->AddChild(horizontal);
			}*/

			//Just naming it the button index
			FName name = FName("Button" + FString::FromInt(i));

			//Create the button
			UbuttonLayout* storItemButton = WidgetTree->ConstructWidget<UbuttonLayout>(buttonTemplate, name);

			//Set up the buttons store items
			storItemButton->SetStoreManager(pStoreManager);
			storItemButton->SetStoreMenu(pMenu);
			storItemButton->SetPurchaseIndex(i);
			//Setup the buttons index
			//p
			//set the buttons text to the item name
			int RanItem  = FMath::RandRange(0, pStoreManager->Items.Num() - 1);
			AStoreItem* item = pStoreManager->Items[RanItem]->GetDefaultObject<AStoreItem>();
			FString strname = FString(item->Getname());
			storItemButton->SetNameText(strname);

			item->RandomizeAmount();
			item->RandomizeCost();

			FString cost = FString("Cost: " + FString::FromInt(item->GetCost()));
			storItemButton->SetCostText(cost);

			FString amount = FString("Amount: " + FString::FromInt(item->GetAmount()));
			storItemButton->SetValueText(amount);

			//horizontal->AddChild(storItemButton);
			buttonList->AddChild(storItemButton);

			/*if (i % 2 == 0)
			{
				buttonList->AddChild(horizontal);
			}*/
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The button template was null"));
	}
}

void UStoreMenuWidget::ExitMenu()
{
	pStoreManager->SetPurchaseInformation(-2); //We have exited the menu
	pStoreManager->PurchaseAvailableItems();
}