// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../Miscellaneous/EndGameEnum.h"
#include "SFHUD.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ASFHUD : public AHUD
{
	GENERATED_BODY()

public:

	ASFHUD();

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf< class UPlayerHUDWidget > pPlayerUITemplate = nullptr;
	UPROPERTY()
		class UPlayerHUDWidget* pPlayerUI = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UPauseMenu> pPauseMenuTemplate = nullptr;
	UPROPERTY()
		class UPauseMenu* pPauseMenu = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UEndGameMenu> pEndGameMenuTemplate = nullptr;
	UPROPERTY()
		class UEndGameMenu* pEndGameMenu = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlayerSelectMenu> pPlayerSelectMenuTemplate = nullptr;
	UPROPERTY()
		class UPlayerSelectMenu* pPlayerSelectMenu = nullptr;


	void OpenPauseMenu();
	void ClosePauseMenu();

	void CreateDefaultPlayerHUD();


	/*
	*The following are used for handling what happens during the end game
	*/
	void OpenEndGameMenu();
	void SetEndGameInformation(const EEndGameReason& reason);

	/*
	*  Will open the player select menu and close the end game menu
	*/
	void OpenPlayerSelectMenu();

protected:
	virtual void BeginPlay() override;

	void CreatePauseMenu();
	
	bool bAutomaticallyCreatePlayerHUD;

};
