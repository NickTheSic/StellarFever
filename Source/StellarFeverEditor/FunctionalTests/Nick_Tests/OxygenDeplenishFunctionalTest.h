// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Components/StatusComponent.h"
#include "OxygenDeplenishFunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AOxygenDeplenishFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()
	
public:

protected:
	virtual void PrepareTest()            override;
	virtual void StartTest()              override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere)
		float fMaxOxygen = 20.0f;
	UPROPERTY(EditAnywhere)
		float fDeplenishRate = 10.0f;

private:
	float fTimeTaken = 0.0f;

	class ASF_PlayerController* pController = nullptr;
	class ABasePlayer*          pPlayer = nullptr;
	class UStatusComponent*     pOxygenComp = nullptr;

};
