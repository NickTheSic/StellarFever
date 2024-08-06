// Copyright Quarantine Games 2020


#include "../OnlinePlayerSelect.h"
#include "../../Miscellaneous/LobbyGamemode.h"
#include "../../Miscellaneous/LobbyPlayerController.h"
#include "../PlayerSelectPieces/PlayerSelectButton.h"


void UOnlinePlayerSelect::NativeConstruct()
{
	Super::NativeConstruct();

	if (ALobbyPlayerController* lobbyPC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		lobbyPC->CheckForSelectedPlayers();
	}
}

void UOnlinePlayerSelect::SetLobbyGameMode(ALobbyGamemode* mode)
{
	pLobbyGamemode = mode;
}

void UOnlinePlayerSelect::SetPlayerClass(const EPlayerClass& playerClass)
{
	Super::SetPlayerClass(playerClass);

	SelectedClass = playerClass;
}

void UOnlinePlayerSelect::SelectPlayer()
{
	if (ALobbyPlayerController* lobbyPC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		lobbyPC->NotifySelection(SelectedClass);
		lobbyPC->CheckForSelectedPlayers();
	}
}

void UOnlinePlayerSelect::DeactivatePlayerButton(const EPlayerClass& playerClass)
{
	UButton* DeactivatedButton = nullptr;

	switch (playerClass)
	{
	case EPlayerClass::Captain:
		CaptainButton->SetIsEnabled(false);
		DeactivatedButton = CaptainButton;
		break;
	case EPlayerClass::Gunner:
		GunnerButton->SetIsEnabled(false);
		DeactivatedButton = GunnerButton;
		break;
	case EPlayerClass::Medic:
		MedicButton->SetIsEnabled(false);
		DeactivatedButton = MedicButton;
		break;
	case EPlayerClass::Engineer:
		EngineerButton->SetIsEnabled(false);
		DeactivatedButton = EngineerButton;
		break;
	}


	//If the deactivated button was the button we had clicked we deactivate it so we can't select the same player
	if (PreviouslyClickedPlayerButton == DeactivatedButton)
	{
		PreviouslyClickedPlayerButton = nullptr;
		SelectButton->SetIsEnabled(false);
	}

}

void UOnlinePlayerSelect::ReactivatePlayerButton(const EPlayerClass& playerClass)
{
	switch (playerClass)
	{
	case EPlayerClass::Captain:
		CaptainButton->SetIsEnabled(true);
		break;
	case EPlayerClass::Gunner:
		GunnerButton->SetIsEnabled(true);
		break;
	case EPlayerClass::Medic:
		MedicButton->SetIsEnabled(true);
		break;
	case EPlayerClass::Engineer:
		EngineerButton->SetIsEnabled(true);
		break;
	}
}

