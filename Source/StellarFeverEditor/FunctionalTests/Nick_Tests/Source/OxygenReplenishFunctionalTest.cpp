// Copyright Quarantine Games 2020

#include "../OxygenReplenishFunctionalTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Components/StatusComponent.h"
#include "Engine/World.h"

void AOxygenReplenishFunctionalTest::PrepareTest() 
{
	Super::PrepareTest();
	LogMessage(TEXT("[OxygenReplenishTest] Prepare Test"));

	pController = Cast<ASF_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (pController == nullptr) FinishTest(EFunctionalTestResult::Error, "pController was null");

	pPlayer = Cast<ABasePlayer>(pController->GetPawn());
	if (pPlayer == nullptr) FinishTest(EFunctionalTestResult::Error, "pPlayer was null");

	pOxygenComp = pPlayer->GetOxygenComponent();
	if (pOxygenComp == nullptr) FinishTest(EFunctionalTestResult::Error, "pOxygenComp was null");

}

void AOxygenReplenishFunctionalTest::StartTest()          
{
	Super::StartTest();
	LogMessage(TEXT("[OxygenReplenishTest] Start Test"));

	//Set the value to zero since we are testing to replenish
	pOxygenComp->SetMaxValue(fMaxOxygen);
	pOxygenComp->SetValue(0);
	pOxygenComp->AddToValue(1.0f);
}

void AOxygenReplenishFunctionalTest::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);

	//pPlayer->ReplenishOxygen(fReplenishRate * DeltaSeconds);

	if (pOxygenComp->IsFull())
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The component is now full in " + FString::SanitizeFloat(fTimeTaken) + " seconds." );
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	if (fTimeTaken > (fMaxOxygen / fReplenishRate))
	{
		LogMessage("Expected: " + FString::SanitizeFloat(fMaxOxygen / fReplenishRate) + " Actual: " + FString::SanitizeFloat(fTimeTaken));
		FinishTest(EFunctionalTestResult::Failed, "Took longer than expected to fill oxygen");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	fTimeTaken += DeltaSeconds;

}