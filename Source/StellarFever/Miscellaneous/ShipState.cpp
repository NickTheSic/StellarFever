// Copyright Quarantine Games 2020

#include "ShipState.h"
#include "../Bay/AmmoBay.h"
#include "../Bay/MedicBay.h"
#include "../Events/LevelProgression/RoundManager.h"
#include "../Events/ShipSystems/Shop/Shipshop.h"

int32 AShipState::GetVaccines() const
{
	return usVaccines;
}

void AShipState::SetVaccines(uint16 amount)
{
	usVaccines = amount;
}

void AShipState::AddVaccines(uint16 amount)
{
	usVaccines += amount;
}

void AShipState::SubVaccines(uint16 amount)
{
	usVaccines -= amount;
}


float AShipState::GetFuelAmount() const
{
	return fFuelAmount;
}

void AShipState::SetFuelAmount(float amount)
{
	fFuelAmount = amount;
	if (pRoundManager != nullptr && pRoundManager->GetShop() != nullptr)
	{
		pRoundManager->SetFuelText(fFuelAmount);
	}
}

void AShipState::AddFuelAmount(float amount)
{
	fFuelAmount += amount;
	if (pRoundManager != nullptr && pRoundManager->GetShop() != nullptr)
	{
		pRoundManager->SetFuelText(fFuelAmount);
	}
}

void AShipState::SubFuelAmount(float amount)
{
	fFuelAmount -= amount;
	if (pRoundManager != nullptr && pRoundManager->GetShop() != nullptr)
	{
		pRoundManager->SetFuelText(fFuelAmount);
	}
}


float AShipState::GetShipHealth() const
{
	return fShipHealth;
}

void AShipState::SetShipHealth(float amount)
{
	fShipHealth = amount;
}

void AShipState::AddShipHealth(float amount)
{
	fShipHealth += amount;
}

void AShipState::SubShipHealth(float amount)
{
	fShipHealth -= amount;
}


int32 AShipState::GetTeamLives() const
{
	return usTeamLives;
}

bool AShipState::HasAnyTeamLivesLeft() const
{
	return usTeamLives > 0;
}

void AShipState::SetTeamLives(uint16 amount)
{
	usTeamLives = amount;
}

void AShipState::AddTeamLives(uint16 amount)
{
	usTeamLives += amount;
}

void AShipState::UseTeamLife()
{
	--usTeamLives;
}


uint64 AShipState::GetMoney() const
{
	return ulMoney;
}

void AShipState::SetMoney(uint64 amount)
{
	ulMoney = amount;
	if (pRoundManager != nullptr && pRoundManager->GetShop() != nullptr)
	{
		pRoundManager->GetShop()->SetCashText(ulMoney);
	}
}

void AShipState::AddMoney(uint64 amount)
{
	ulMoney += amount;
	if (pRoundManager != nullptr && pRoundManager->GetShop() != nullptr)
	{
		pRoundManager->GetShop()->SetCashText(ulMoney);
	}
}

void AShipState::SubMoney(uint64 amount)
{
	ulMoney -= amount;
	if (pRoundManager != nullptr && pRoundManager->GetShop() != nullptr)
	{
		pRoundManager->GetShop()->SetCashText(ulMoney);
	}
}

AAmmoBay* AShipState::GetAmmoReserve() const
{
	return pAmmoBay;
}

void AShipState::SetAmmoReserve(class AAmmoBay* ammoBay)
{
	pAmmoBay = ammoBay;
}

void AShipState::AddAmmoReserve(uint64 amount)
{
	pAmmoBay->PartialRefill(amount);
}

AMedicBay* AShipState::GetMedReserves() const
{
	return pMedBay;
}

void AShipState::SetMedReserves(class AMedicBay* medbay)
{
	pMedBay = medbay;
}

void AShipState::AddMedReserves(uint64 amount)
{
	pMedBay->PartialRefill(amount);
}
