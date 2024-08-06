// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "UseSpecialBase_FunctionalTest.h"
#include "UseSpecialCaptain_FunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVEREDITOR_API AUseSpecialCaptain_FunctionalTest : public AUseSpecialBase_FunctionalTest
{
	GENERATED_BODY()
protected:

	virtual void PrepareTest()            override;
	virtual void StartTest()              override;

private:

	FVector startingLocation;
	FVector endingLocation;

	class ACaptainPlayer* pCaptain = nullptr;
};
