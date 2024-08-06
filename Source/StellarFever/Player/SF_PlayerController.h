// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerInformation.h"
#include "../Miscellaneous/EndGameEnum.h"
#include "SF_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API ASF_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASF_PlayerController();

	virtual void OnPossess(class APawn* InPawn) override;

	void SetupInputComponent() override;


	//Movement Functions
	void Jump();
	void StopJumping();

	void StartInteraction();
	void StopInteraction();

	void StartRunning();
	void StopRunning();

	void MoveForward(float val);
	void MoveRight(float val);

	void PlayerBeginCrouch();
	void PlayerEndCrouch(); 

	void Reload();
	void Fire();
	void StopFire();
	void AltFire();
	void StopAltFire();
	void StartVoice();
	void EndVoice();

	void ClassSpecial();
	void Throw();


	//Inventory Switching
	void MouseWheelUp();
	void MouseWheelDown();
	void NumKeyOne();
	void NumKeyTwo();
	void NumKeyThree();
	
	//Look speed
	void UpdateLookSpeed(float speed);


	//Menu
	void PauseMenu();

	void OpenPauseMenu();
	void ClosePauseMenu();


	//Overriding so I can check if we are paused
	virtual void AddYawInput(float val) override;
	virtual void AddPitchInput(float val) override;


	bool GetIsInAMenu() { return bInPauseMenu || bInGameOver; }

	/**
	* The following are used to interact with the End Game Screen widget and the Game mode
	*/

	void TriggerEndGame(const EEndGameReason& endGameReason);

	UFUNCTION(Client, Reliable)
		void Client_EnableEndGameMenu(const EEndGameReason& endGameReason);

	void PlayerReturnToMenu();
	UFUNCTION(Server, Reliable)
		void Server_PlayerReturnToMenu();
	void PlayerPlayAgain();
	UFUNCTION(Server, Reliable)
		void Server_PlayerPlayAgain();


protected:

	UPROPERTY(Replicated)
		class ABasePlayer* pPlayer = nullptr;

	void BeginPlay() override;

	bool bInPauseMenu = false;
	UPROPERTY(Replicated)
		bool bInGameOver  = false;

	UFUNCTION(Exec)
		void SendMessage(FString message);

	UFUNCTION(Server, Reliable)
		void SendMessageToServer(const FString& message, const FString& sender);

};
