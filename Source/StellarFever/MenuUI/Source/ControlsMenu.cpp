// Copyright Quarantine Games 2020


#include "../ControlsMenu.h"
#include "../MainMenuSwitcher.h"
#include "../../PauseMenu/PauseMenu.h"
#include "Components/Button.h"

void UControlsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UControlsMenu::OnClickBack);
		_Buttons.Add(BackButton);
	}

}

void UControlsMenu::OnClickBack()
{
	if (pMainMenuSwitcher != nullptr)
	{
		pMainMenuSwitcher->SwitchToMainMenu();
	}

	if (pPauseMenuSwitcher != nullptr)
	{
		pPauseMenuSwitcher->SwitchToMainMenu();
	}
}
