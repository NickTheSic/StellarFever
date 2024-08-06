// Copyright Quarantine Games 2020


#include "MedBayRefills.h"
#include "../../Miscellaneous/ShipState.h"

void AMedBayRefills::Purchase()
{
	if (pshipstate->GetMoney() >= iCost)
	{
		pshipstate->AddMedReserves(iAmount);
		pshipstate->SubMoney(iCost);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "You have purchased ammo for cost " + FString::FromInt(iCost));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "with Amount of  " + FString::FromInt(iAmount));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("purchase denied!!"));
	}
}