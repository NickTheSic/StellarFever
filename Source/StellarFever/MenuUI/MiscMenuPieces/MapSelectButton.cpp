// Copyright Quarantine Games 2020


#include "MapSelectButton.h"
#include "../MainMenuGameMode.h"
#include "../MainMenuSwitcher.h"
#include "../MainMenuPlayerController.h"
#include "../MainMenuHUD.h"


void UMapSelectButton::Setup(FName mapName, UMainMenuSwitcher* menuSwitcher)
{
	MapName = mapName;
	OnClicked.AddDynamic(this, &UMapSelectButton::OnClick);

	pMainMenuSwitcher = menuSwitcher;
}

void UMapSelectButton::OnClick()
{
	if (AMainMenuGameMode* mainMenuGameMode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode()))
	{
		mainMenuGameMode->SetLoadMap(MapName);
	}


	//Load the player select menu after selecting a map
	if (pMainMenuSwitcher != nullptr)
	{
		pMainMenuSwitcher->SwitchToPlayerSelect();
	}
	else	//Get the hud to get the main menu swticher object
	{
		if (AMainMenuPlayerController* pc = Cast<AMainMenuPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (AMainMenuHUD* hud = Cast<AMainMenuHUD>(pc->GetHUD()))
			{
				pMainMenuSwitcher = Cast<UMainMenuSwitcher>(hud->pMainMenuObject);
				if (pMainMenuSwitcher != nullptr)
				{
					pMainMenuSwitcher->SwitchToPlayerSelect();
				}
			}
		}
	}

}
