// Copyright Quarantine Games 2020


#include "VaccineRefills.h"
#include "../../Miscellaneous/ShipState.h"

void AVaccineRefills::Purchase()
{
	if (pshipstate->GetMoney() >= iCost)
	{
		pshipstate->AddVaccines(iAmount);
		pshipstate->SubMoney(iCost);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "You have purchased ammo for cost " + FString::FromInt(iCost));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "with Amount of  " + FString::FromInt(iAmount));

		//lan
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("purchase denied!!"));
	}
}