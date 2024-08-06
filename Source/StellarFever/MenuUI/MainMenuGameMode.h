// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	TArray<FName> GetAllMaps();

	UPROPERTY(EditAnywhere, Category = "Level Select")
		FName MapToLoad = "Ship1GB";

	//Cheat code for selecting map
	UFUNCTION(Exec)
		void SetLoadMap(FName map);
};
