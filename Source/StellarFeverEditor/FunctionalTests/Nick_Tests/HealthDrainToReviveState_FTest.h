// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "HealthDrainToReviveState_FTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AHealthDrainToReviveState_FTest : public AFunctionalTest
{
	GENERATED_BODY()

public:
	virtual void PrepareTest() override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:

	float fTimer = 0.0f;

	class UHealthComponent* pHealthComp = nullptr;
	class ABasePlayer*      pPlayer     = nullptr;
};
