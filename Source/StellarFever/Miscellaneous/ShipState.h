// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ShipState.generated.h"

/**
 * 
 */
 

UCLASS(BlueprintType, Blueprintable)
class STELLARFEVER_API AShipState : public AInfo
{
	GENERATED_BODY()
protected:
	uint16 usVaccines = 0;
	uint16 usTeamLives = 0;
	uint64 ulMoney = 0;
	float fFuelAmount = 0.f;
	float fShipHealth = 0.f;

	UPROPERTY()
	class AAmmoBay* pAmmoBay;

	UPROPERTY()
	class AMedicBay* pMedBay;

	UPROPERTY()
	class ARoundManager* pRoundManager;

public:

	UFUNCTION(BlueprintCallable)
	int32 GetVaccines() const;
	void SetVaccines(uint16 amount);
	void AddVaccines(uint16 amount);
	void SubVaccines(uint16 amount);

	UFUNCTION(BlueprintCallable)
	float GetFuelAmount() const;
	void SetFuelAmount(float amount);
	void AddFuelAmount(float amount);
	void SubFuelAmount(float amount);

	UFUNCTION(BlueprintCallable)
	float GetShipHealth() const;
	void SetShipHealth(float amount);
	void AddShipHealth(float amount);
	void SubShipHealth(float amount);

	UFUNCTION(BlueprintCallable)
	int32 GetTeamLives() const;
	UFUNCTION(BlueprintCallable)
	bool HasAnyTeamLivesLeft() const;
	void SetTeamLives(uint16 amount);
	void AddTeamLives(uint16 amount);
	void UseTeamLife();

	uint64 GetMoney() const;
	void SetMoney(uint64 amount);
	void AddMoney(uint64 amount);
	void SubMoney(uint64 amount);

	class AAmmoBay* GetAmmoReserve() const;
	void SetAmmoReserve(class AAmmoBay* ammoBay);
	void AddAmmoReserve(uint64 amount);

	class AMedicBay* GetMedReserves() const;
	void SetMedReserves(class AMedicBay* medbay);
	void AddMedReserves(uint64 amount);

	void SetRoundManager(ARoundManager* Roundmanager) { pRoundManager = Roundmanager; }
	
};
