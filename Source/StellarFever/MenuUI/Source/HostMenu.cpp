// Copyright Quarantine Games 2020


#include "../HostMenu.h"
#include "../MainMenuSwitcher.h"
#include "../../StellarFeverGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"


void UHostMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UHostMenu::OnClickBack);
		_Buttons.Add(BackButton);
	}

	if (StartHostingButton)
	{
		StartHostingButton->OnClicked.AddDynamic(this, &UHostMenu::HostServer);
		_Buttons.Add(StartHostingButton);
	}

}


void UHostMenu::OnClickBack()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToMainMenu();
	}
}

void UHostMenu::HostServer()
{
	if (UStellarFeverGameInstance* sfInstance = GetGameInstance<UStellarFeverGameInstance>())
	{
		pMainMenuSwitcher->SwitchToLoadingScreen();

		FString ServerName = ServerNameText->Text.ToString();
		FString Map = "";
		sfInstance->Host(ServerName, Map);
	}
}
