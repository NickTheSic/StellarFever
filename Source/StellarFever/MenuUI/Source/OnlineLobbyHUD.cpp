// Copyright Quarantine Games 2020


#include "../OnlineLobbyHUD.h"
#include "../OnlinePlayerSelect.h"
#include "../../Miscellaneous/LobbyGamemode.h"

AOnlineLobbyHUD::AOnlineLobbyHUD()
{
	bAutomaticallyCreatePlayerHUD = false;
}

void AOnlineLobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (OnlinePlayerSelectTemplate)		
	{
		pPlayerSelect = CreateWidget<UOnlinePlayerSelect>(GetWorld(), OnlinePlayerSelectTemplate);

		pPlayerSelect->pLobbyGamemode = GetWorld()->GetAuthGameMode<ALobbyGamemode>();
		
		pPlayerSelect->AddToViewport();
	}
}
