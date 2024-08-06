// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "UseSpecialBase_FunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AUseSpecialBase_FunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()

protected:
	virtual void PrepareTest() override;
	virtual void StartTest()   override;

	class ASF_PlayerController* pController = nullptr;
	class ABasePlayer* pPlayer = nullptr;

};
