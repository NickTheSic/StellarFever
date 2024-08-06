// Copyright Quarantine Games 2020


#include "../MainMenuHUD.h"
#include "../../StellarFeverGameInstance.h"
#include "../MainMenu.h"
#include "../MainMenuSwitcher.h"

void AMainMenuHUD::SetEnablePlay()
{
	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (sfInstance)
	{
		sfInstance->bTutorialComplete = true;
	}
	
	if (UMainMenuSwitcher* pmmo = Cast<UMainMenuSwitcher>(pMainMenuObject))
	{
		if (pmmo->pMainMenu != nullptr)
		{
			pmmo->pMainMenu->SetEnablePlay();
		}
	}
}

void AMainMenuHUD::GoBack()
{
	if (pMainMenuObject)
	{
		pMainMenuObject->SwitchToMainMenu();
	}
}


void AMainMenuHUD::BeginPlay()
{
	if (MainMenuSwitcherTemplate)
	{
		pMainMenuObject = CreateWidget<UMainMenuSwitcher>(GetWorld(), MainMenuSwitcherTemplate);
		if (pMainMenuObject)
		{
			pMainMenuObject->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create main menu widget"));
		}
	}
}


