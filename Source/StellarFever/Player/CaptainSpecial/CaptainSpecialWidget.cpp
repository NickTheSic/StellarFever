// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/

#include "CaptainSpecialWidget.h"
#include "../CaptainPlayer.h"
#include "Blueprint/WidgetTree.h"
#include "CaptainSpecialScreen.h"
#include "CaptainTeleportButton.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"



UCaptainSpecialWidget::UCaptainSpecialWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	buttonTemplate = UCaptainTeleportButton::StaticClass();
}

void UCaptainSpecialWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (exitButton)
	{
		exitButton->OnClicked.AddDynamic(this, &UCaptainSpecialWidget::ExitMenu);
		exitButton->SetPressMethod(EButtonPressMethod::ButtonPress);
	}
}

void UCaptainSpecialWidget::CreateButtons(int32 numButtons)
{

	if (numButtons == 0)
	{
		//Set the title text to say there isn't any places
		titleText->SetText(FText::FromString(TEXT("No locations set")));
		FWidgetTransform trans;
		trans.Translation.Y = 58;
		trans.Translation.X = 30;
		titleText->Slot->Content->SetRenderTransform(trans);
		return;
	}

	if (buttonTemplate != nullptr)
	{
		for (int i = 0; i < numButtons; i++)
		{
			//Just naming it the button index
			FName name = FName("Button" + FString::FromInt(i));

			//Create the button
			UCaptainTeleportButton* teleportButton = WidgetTree->ConstructWidget<UCaptainTeleportButton>(buttonTemplate, name);

			//Set up the buttons captain
			teleportButton->SetCaptain(pCaptain);
			teleportButton->SetCaptainScreen(pScreen);
			//Setup the buttons index
			teleportButton->SetTeleportIndex(i);

			//Create the text object in the button
			teleportButton->CreateTextSubWidget(WidgetTree);
			//set the buttons text to the target spot name
			FString string = pCaptain->targetSpots[i]->GetFName().ToString();
			teleportButton->SetButtonText(string);

			buttonList->AddChild(teleportButton);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The button template was null"));
	}
}

void UCaptainSpecialWidget::ExitMenu()
{
	pCaptain->SetTeleportInformation(-2); //We have exited the menu
	pCaptain->ClassSpecial(); //With how I set it up we have to call ClassSpecial to exit the menu;
}
