// Copyright Quarantine Games 2020


#include "../PlayerSelectMenu.h"
#include "../../StellarFeverGameInstance.h"
#include "../MainMenuGameMode.h"
#include "../MainMenuHUD.h"
#include "../MainMenuSwitcher.h"
#include "../PlayerSelectPieces/PlayerInformationDisplayBox.h"
#include "../PlayerSelectPieces/PlayerSelectButton.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPlayerSelectMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UPlayerSelectMenu::GoBack);
		_Buttons.Add(BackButton);
	}
}


void UPlayerSelectMenu::SetPlayerClass(const EPlayerClass& playerClass)
{
	Super::SetPlayerClass(playerClass);

	UStellarFeverGameInstance* gameInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		gameInstance->SetIsOnlineGame(false);
		gameInstance->PlayerOneSelectedClass = playerClass;
		gameInstance->bPlayerWasChosen = true;
	}
}


void UPlayerSelectMenu::GoBack()
{
	pMainMenuSwitcher->SwitchToMainMenu();
}


void UPlayerSelectMenu::SelectPlayer()
{
	AMainMenuGameMode* gamemode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());

	if (gamemode)
		UGameplayStatics::OpenLevel(GetWorld(), gamemode->MapToLoad, true, "?Game=/Game/Blueprints/BP_SinglePlayerGameMode.BP_SinglePlayerGameMode_C"); //Load the map from the game mode if it exists

	else
		UGameplayStatics::OpenLevel(GetWorld(), "ShipWithSystemsV3", true, "?Game=/Game/Blueprints/BP_SinglePlayerGameMode.BP_SinglePlayerGameMode_C"); //Load a default map
}
