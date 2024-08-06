// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../StellarFeverGameModeBase.h"
#include "../Player/PlayerInformation.h"
#include "StellarFeverMultiplayerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AStellarFeverMultiplayerGameMode : public AStellarFeverGameModeBase
{
	GENERATED_BODY()

public:

	virtual APlayerController* Login(UPlayer* NewPlayer,
		ENetRole InRemoteRole,
		const FString& Portal,
		const FString& Options,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;


	virtual void Logout(AController* Exiting) override;


	void PlayerMadeEndGameChoice(bool bWantsToContinue);


protected:

	int iPlayerEndGameChoiceCount = 0;
	int iActivePlayersInGame = 0;

	void GetPlayerClassFromType(const EPlayerClass& InClass);

	TArray<EPlayerClass> AvailableClasses = { EPlayerClass::Captain, EPlayerClass::Engineer, EPlayerClass::Gunner, EPlayerClass::Medic };

};
