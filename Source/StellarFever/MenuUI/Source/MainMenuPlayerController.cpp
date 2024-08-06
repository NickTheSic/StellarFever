// Copyright Quarantine Games 2020


#include "../MainMenuPlayerController.h"
#include "../MainMenuHUD.h"
#include "../MainMenuSwitcher.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	//We want to see the mouse while in the MainMenu
	bShowMouseCursor = true;

}

void AMainMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("MenuBack", IE_Pressed, this, &AMainMenuPlayerController::MenuGoBack);

}

void AMainMenuPlayerController::BeginPlay()
{
	//We want input to be UI Only for the main menu
	FInputModeGameAndUI inputMode;
	SetInputMode(inputMode);

	//We don't want to move while we are in the main menu
	SetIgnoreLookInput(true);
	SetIgnoreMoveInput(true);
}

void AMainMenuPlayerController::MenuGoBack()
{
	if (AMainMenuHUD* MainMenu = GetHUD<AMainMenuHUD>())
	{
		MainMenu->GoBack();
	}
}
