// Copyright Quarantine Games 2020


#include "../MainMenu.h"
#include "../../StellarFeverGameInstance.h"
#include "../MainMenuHUD.h"
#include "../MainMenuSwitcher.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Bind our button clicks to the corresponding functions
	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickPlay);
		_Buttons.Add(PlayButton);
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickHost);
		_Buttons.Add(HostButton);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickJoin);
		_Buttons.Add(JoinButton);
	}

	if (TutorialButton)
	{
		TutorialButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickTutorial);
		_Buttons.Add(TutorialButton);
	}

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickOptions);
		_Buttons.Add(OptionsButton);
	}

	if (ControlsButton)
	{
		ControlsButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickControls);
		_Buttons.Add(ControlsButton);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickExit);
		_Buttons.Add(ExitButton);
	}

	if (MapSelectButton)
	{
#if WITH_EDITOR
		MapSelectButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickMapSelect);
		_Buttons.Add(MapSelectButton);
#else 
		MapSelectButton->SetIsEnabled(false);
		MapSelectButton->SetVisibility(ESlateVisibility::Hidden);
#endif
	}

}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//When the menu loads on screen we can check if we have beaten the Tutorial
	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (sfInstance != nullptr)
	{
		bool IsButtonEnabled;
		
#if WITH_EDITOR
		IsButtonEnabled = true;
#else
		IsButtonEnabled = sfInstance->bTutorialComplete;
#endif

		if (PlayButton) PlayButton->SetIsEnabled(IsButtonEnabled);
		if (HostButton) HostButton->SetIsEnabled(IsButtonEnabled);
		if (JoinButton) JoinButton->SetIsEnabled(IsButtonEnabled);

		sfInstance->SetIsOnlineGame(false);
	}
}


void UMainMenu::OnClickExit()
{
	//If In game 
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		GetWorld()->GetFirstPlayerController(),
		EQuitPreference::Quit, 
		false);
}

void UMainMenu::OnClickPlay()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToPlayerSelect();
	}
}

void UMainMenu::OnClickTutorial()
{
	UGameplayStatics::OpenLevel(GetWorld(), "TutorialMap2");
	
#if !(UE_BUILD_SHIPPING)
	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (sfInstance != nullptr)
	{
		sfInstance->bTutorialComplete = true;
	}
#endif
}


void UMainMenu::OnClickOptions()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToOptionsMenu();
	}
}

void UMainMenu::OnClickControls()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToControlsMenu();
	}
}

void UMainMenu::OnClickHost()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToHostMenu();
	}
}


void UMainMenu::OnClickJoin()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToJoinMenu();
	}
}

void UMainMenu::OnClickMapSelect()
{
	if (pMainMenuSwitcher != nullptr)
	{
		pMainMenuSwitcher->SwitchToMapSelect();
	}
}


void UMainMenu::SetEnablePlay()
{
	if (PlayButton)
	{
		PlayButton->SetIsEnabled(true);
	}
	if (HostButton)
	{
		HostButton->SetIsEnabled(true);
	}
	if (JoinButton)
	{
		JoinButton->SetIsEnabled(true);
	}
}