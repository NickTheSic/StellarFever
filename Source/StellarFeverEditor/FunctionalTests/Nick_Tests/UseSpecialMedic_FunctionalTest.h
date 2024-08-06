// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "UseSpecialBase_FunctionalTest.h"
#include "UseSpecialMedic_FunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AUseSpecialMedic_FunctionalTest : public AUseSpecialBase_FunctionalTest
{
	GENERATED_BODY()
	
protected:
	virtual void PrepareTest()            override;
	virtual void StartTest()              override;
	virtual void Tick(float DeltaSeconds) override;

private:

	float fTimer    = 0.0f;
	float fHealRate = 4.0f;
	float fHealTime = 15.0f;

	class AMedicPlayer*     pMedic      = nullptr;
	class UHealthComponent* pHealthComp = nullptr;
};
