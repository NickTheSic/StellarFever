// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../Player/SF_PlayerController.h"
#include "../Player/PlayerInformation.h"
#include "LobbyPlayerController.generated.h"

/**
 * This class is our regular player controller with some added functions that are needed in the Lobby specifically
 */
UCLASS()
class STELLARFEVER_API ALobbyPlayerController : public ASF_PlayerController
{
	GENERATED_BODY()
	
public:

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(Client, Reliable)
		virtual void ClientCheckPossession(APawn* InPawn);

	UFUNCTION(Client, Reliable)
		void Client_DeactivateButton(const EPlayerClass& playerClass);

	UFUNCTION(Client, Unreliable)
		void ClientReactivateButton(const EPlayerClass& InPlayerClass);

	
	void NotifySelection(const EPlayerClass& playerClass);
	UFUNCTION(Server, Reliable)
		void Server_NotifySelection(APlayerController* playerController, const EPlayerClass& playerClass);

	void SetToGameInput();

	void SetOnlinePlayerIndex(uint16 index) { uiOnlinePlayerIndex = index; }
	uint16 GetOnlinePlayerIndex() { return uiOnlinePlayerIndex; }

	UFUNCTION(Server, Unreliable)
	void CheckForSelectedPlayers();

	UFUNCTION(Client, Reliable)
		void Client_DisplayLoadingScreen();

protected:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> LoadingScreenTemplate = nullptr;

	virtual void BeginPlay() override;

	uint16 uiOnlinePlayerIndex = 0;

};
