// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MessageSystem/MessageContainer.h"
#include "StellarFeverGameState.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API AStellarFeverGameState : public AGameStateBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(replicated)
	class AShipState* pShipState = nullptr;

public:
	AStellarFeverGameState();
	UFUNCTION(BlueprintCallable)
	AShipState* GetShipState();

	UPROPERTY(replicated)
		UMessageContainer* pMessageContainer = nullptr;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
