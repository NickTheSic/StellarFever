// Copyright Quarantine Games 2020


#include "PauseMenu.h"
#include "../MenuUI/OptionsMenu.h"
#include "../MenuUI/ControlsMenu.h"
#include "../Player/SF_PlayerController.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"


void UPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::ResumeGame);
		_Buttons.Add(ResumeButton);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UPauseMenu::ExitGame);
		_Buttons.Add(ExitButton);
	}

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UPauseMenu::SwitchToOptions);
		_Buttons.Add(OptionsButton);
	}

	if (ControlsButton)
	{
		ControlsButton->OnClicked.AddDynamic(this, &UPauseMenu::SwitchToControlsMenu);
		_Buttons.Add(ControlsButton);
	}

	if (pOptionsMenu)
	{
		pOptionsMenu->pPauseMenuSwitcher = this;
	}

	if (pControlsMenu)
	{
		pControlsMenu->pPauseMenuSwitcher = this;
	}
}

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//Make sure the main menu is on screen when this appears on screen
	SwitchToMainMenu();

}

void UPauseMenu::ResumeGame()
{
	if (ASF_PlayerController* pc = GetOwningPlayer<ASF_PlayerController>())
	{
		pc->ClosePauseMenu();
	}
}

void UPauseMenu::ExitGame()
{
	UGameplayStatics::OpenLevel(GetOwningPlayer(), "MainMenuMap");
}

void UPauseMenu::SwitchToOptions()
{
	if (pPauseMenuSwitcher)
	{
		pPauseMenuSwitcher->SetActiveWidget(pOptionsMenu);
	}

	if(pOptionsMenu && pOptionsMenu->ApplySettingsButton)
	{
		pOptionsMenu->ApplySettingsButton->SetFocus();
	}
}

void UPauseMenu::SwitchToMainMenu()
{
	if (pPauseMenuSwitcher)
	{
		if (PauseMenuPanel)
			pPauseMenuSwitcher->SetActiveWidget(PauseMenuPanel);
		else
			UE_LOG(LogTemp, Error, TEXT("The Pause Menu canvas panel was null "));
	}

	if (ResumeButton->IsValidLowLevel())
	{
		ResumeButton->SetFocus();
	}
}

void UPauseMenu::SwitchToControlsMenu()
{
	if (pPauseMenuSwitcher)
	{
		pPauseMenuSwitcher->SetActiveWidget(pControlsMenu);
	}

	if (pControlsMenu && pControlsMenu->BackButton)
	{
		pControlsMenu->BackButton->SetFocus();
	}
}

