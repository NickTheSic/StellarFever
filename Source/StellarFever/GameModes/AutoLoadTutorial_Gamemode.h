// Copyright Quarantine Games 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AutoLoadTutorial_Gamemode.generated.h"

/**
 * This game mode is to be placed in an empty start up map.
 * The goal is to allow users to open the game and get sent to the tutorial if they haven't completed it
 */
UCLASS()
class STELLARFEVER_API AAutoLoadTutorial_Gamemode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

};
