// Copyright Quarantine Games 2020


#include "StellarFeverGameState.h"
#include "Miscellaneous/ShipState.h"
#include "Net/UnrealNetwork.h"

AStellarFeverGameState::AStellarFeverGameState()
{
	pShipState = CreateDefaultSubobject<AShipState>("ShipState");
	pMessageContainer = CreateDefaultSubobject<UMessageContainer>("Messages");
	pMessageContainer->SetIsReplicated(true);
}

AShipState* AStellarFeverGameState::GetShipState()
{
	return pShipState;
}

void AStellarFeverGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStellarFeverGameState, pShipState);
	DOREPLIFETIME(AStellarFeverGameState, pMessageContainer);
}