// Copyright Quarantine Games 2020


#include "../MainMenuSwitcher.h"
#include "../ControlsMenu.h"
#include "../HostMenu.h"
#include "../JoinMenu.h"
#include "../MainMenu.h"
#include "../MapSelectMenu.h"
#include "../OptionsMenu.h"
#include "../PlayerSelectMenu.h"
#include "../PlayerSelectPieces/PlayerSelectButton.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"

void UMainMenuSwitcher::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Setup the MainMenu
	if (pMainMenu != nullptr)
	{
		pMainMenu->pMainMenuSwitcher = this;
	}

	//Setup the Player Select Menu
	if (pPlayerSelectMenu != nullptr)
	{
		pPlayerSelectMenu->pMainMenuSwitcher = this;
	}

	if (pOptionsMenu)
	{
		pOptionsMenu->pMainMenuSwitcher = this;
	}

	if (pHostMenu)
	{
		pHostMenu->pMainMenuSwitcher = this;
	}

	if (pJoinMenu)
	{
		pJoinMenu->pMainMenuSwitcher = this;
	}

	if (pMapSelectMenu != nullptr)
	{
		pMapSelectMenu->pMainMenuSwitcher = this;
	}

	if (pControlsMenu != nullptr)
	{
		pControlsMenu->pMainMenuSwitcher = this;
	}

#if WITH_EDITOR
	if (pMenuSwitcher != nullptr)
	{
		SwitchToMainMenu();
	}
#endif
}

void UMainMenuSwitcher::NativeConstruct()
{
	Super::NativeConstruct();

	if (pMainMenu)
	{
		pMainMenu->TutorialButton->SetFocus();
	}
}

void UMainMenuSwitcher::SwitchToMainMenu()
{
	pMenuSwitcher->SetActiveWidget( pMainMenu );
	pMainMenu->TutorialButton->SetFocus();
}

void UMainMenuSwitcher::SwitchToPlayerSelect()
{
	pMenuSwitcher->SetActiveWidget( pPlayerSelectMenu );
	pPlayerSelectMenu->OpenMenu();
}

void UMainMenuSwitcher::SwitchToOptionsMenu()
{
	pMenuSwitcher->SetActiveWidget( pOptionsMenu );
	pOptionsMenu->ApplySettingsButton->SetFocus();
}

void UMainMenuSwitcher::SwitchToHostMenu()
{
	pMenuSwitcher->SetActiveWidget(pHostMenu);
	pHostMenu->StartHostingButton->SetFocus();
}

void UMainMenuSwitcher::SwitchToJoinMenu()
{
	//Refresh the server list before going into the menu so the player doesn't have to hit refresh
	pJoinMenu->RefreshServers(); 
	pMenuSwitcher->SetActiveWidget(pJoinMenu);
	pJoinMenu->JoinButton->SetFocus();
}

void UMainMenuSwitcher::SwitchToMapSelect()
{
	pMenuSwitcher->SetActiveWidget(pMapSelectMenu);
}

void UMainMenuSwitcher::SwitchToControlsMenu()
{
	pMenuSwitcher->SetActiveWidget(pControlsMenu);
	pControlsMenu->BackButton->SetFocus();
}

void UMainMenuSwitcher::SwitchToLoadingScreen()
{
	pMenuSwitcher->SetActiveWidget(pLoadingScreen);
}
