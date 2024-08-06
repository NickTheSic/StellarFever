// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "StaminaDrainFunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AStaminaDrainFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()
	
protected:
	virtual void PrepareTest()            override;
	virtual void StartTest()              override;
	virtual void Tick(float DeltaSeconds) override;

private:

	float fMaxStamina = 100.0f;
	float fDrainRate  = 30.0f;
	float fTimeTaken  = 0.0f;

	class ASF_PlayerController* pController  = nullptr;
	class ABasePlayer*          pPlayer      = nullptr;
	class UStatusComponent*     pStaminaComp = nullptr;

};
