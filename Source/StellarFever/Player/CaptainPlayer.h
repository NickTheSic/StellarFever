// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "BasePlayer.h"
#include "CaptainPlayer.generated.h"

/**
 * This is the outline for the Captain player
 * Most complex special setup that is related to the BasePlayer
 */
UCLASS()
class STELLARFEVER_API ACaptainPlayer : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	ACaptainPlayer();


	bool UseSpecial() override;

	UPROPERTY(EditAnywhere, Category = "TargetPoints")
		TArray<AActor*> targetSpots;

	//This is how we are going to set what the player has selected
	void SetTeleportInformation(int idx);
	UFUNCTION(Server, Reliable)
		void Server_SetTeleportInformation(int idx);


	void StartRageMode();
	void EndRageMode();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UTeleportActorComponent* pTeleportComponent = nullptr;

private:
	void LeaveCaptainSpecialMenu();
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_LeaveCaptainSpecialMenu();

	void EnterCaptainSpecialMenu();
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_EnterCaptainSpecialMenu();


	void TeleportTheCaptain();
	UFUNCTION(Server, Reliable)
		void Server_TeleportTheCaptain();

	UPROPERTY(EditAnywhere, Category = "Player | Captain Components")
		class UCaptainScreen* pCaptainScreen;

	/*
	* -1  : Hasn't been selected
	* -2  : Player Cancelled
	* 0++ : The teleport spot to jump to
	*/
	UPROPERTY(Replicated)
		int8 iCaptainTeleportInformation = -1;

};
