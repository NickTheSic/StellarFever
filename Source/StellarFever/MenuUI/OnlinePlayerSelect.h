// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "PlayerSelectMenuBase.h"
#include "../Player/PlayerInformation.h"
#include "OnlinePlayerSelect.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UOnlinePlayerSelect : public UPlayerSelectMenuBase
{
	GENERATED_BODY()

public:
	class ALobbyGamemode* pLobbyGamemode = nullptr;
	class APlayerController* pOwningPlayer = nullptr;

	EPlayerClass SelectedClass;

public:

	virtual void NativeConstruct() override;

	void SetLobbyGameMode(class ALobbyGamemode* mode);

	virtual void SetPlayerClass(const EPlayerClass& playerClass);

	virtual void SelectPlayer();

	void DeactivatePlayerButton(const EPlayerClass& playerClass);
	void ReactivatePlayerButton(const EPlayerClass& playerClass);
};
