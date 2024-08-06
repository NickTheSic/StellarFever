// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../StellarFeverGameModeBase.h"
#include "StellarFeverSinglePlayerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AStellarFeverSinglePlayerGameMode : public AStellarFeverGameModeBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Trash Bags")
		TSubclassOf<AActor> TrashBagTemplate;

	UPROPERTY(EditAnywhere, Category = "Trash Bags")
		TArray<FVector> TrashBagSpawnLocations;


	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:

	virtual void BeginPlay() override;

};
