// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "OxygenReplenishFunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AOxygenReplenishFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()
public:

protected:
	virtual void PrepareTest()            override;
	virtual void StartTest()              override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere)
		float fMaxOxygen     = 20.0f;
	UPROPERTY(EditAnywhere)
		float fReplenishRate = 10.0f;

private:
	float fTimeTaken = 0.0f;

	class ASF_PlayerController* pController = nullptr;
	class ABasePlayer*          pPlayer     = nullptr;
	class UStatusComponent*     pOxygenComp = nullptr;

};
