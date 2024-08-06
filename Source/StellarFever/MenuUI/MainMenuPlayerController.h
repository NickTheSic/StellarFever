// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 *  We need a player controller just for UI Purposes
 *  and this is that controller
 */
UCLASS()
class STELLARFEVER_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainMenuPlayerController();

	void SetupInputComponent() override;

	virtual void BeginPlay() override;

	void MenuGoBack();
};
