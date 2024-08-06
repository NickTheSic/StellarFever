// Copyright Quarantine Games 2020


#include "AmmoBayRefills.h"
#include "../../Miscellaneous/ShipState.h"
#include "Engine/Engine.h"

void AAmmoBayRefills::Purchase()
{
	if (pshipstate->GetMoney() >= iCost)
	{
		pshipstate->AddAmmoReserve(iAmount);
		pshipstate->SubMoney(iCost);
		UE_LOG(LogTemp, Warning, TEXT("purchased ammobay refills"));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "You have purchased ammo for cost " +FString::FromInt(iCost));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "with Amount of  " + FString::FromInt(iAmount));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ammo refills denied"));
	}
}