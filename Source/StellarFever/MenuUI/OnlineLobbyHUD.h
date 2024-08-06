// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../Player/SFHUD.h"
#include "OnlineLobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AOnlineLobbyHUD : public ASFHUD
{
	GENERATED_BODY()
	
public:

	AOnlineLobbyHUD();

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UOnlinePlayerSelect> OnlinePlayerSelectTemplate;

	class UOnlinePlayerSelect* pPlayerSelect = nullptr;

protected:

	virtual void BeginPlay() override;
};
