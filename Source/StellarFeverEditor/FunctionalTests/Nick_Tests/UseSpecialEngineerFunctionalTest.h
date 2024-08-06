// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "UseSpecialBase_FunctionalTest.h"
#include "UseSpecialEngineerFunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AUseSpecialEngineerFunctionalTest : public AUseSpecialBase_FunctionalTest
{
	GENERATED_BODY()
protected:

	virtual void PrepareTest()            override;
	virtual void StartTest()              override;

private:

	class AEngineerPlayer* pEngineer = nullptr;
};
