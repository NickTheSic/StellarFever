// Copyright Quarantine Games 2020

#include "../OxygenDeplenishFunctionalTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Components/StatusComponent.h"
#include "Engine/World.h"

void AOxygenDeplenishFunctionalTest::PrepareTest()
{
	Super::PrepareTest();

	LogMessage(TEXT("[OxygenDeplenishFunctionalTest] Prepare Test"));

	pController = Cast<ASF_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (pController == nullptr) FinishTest(EFunctionalTestResult::Error, "pController was null");

	pPlayer = Cast<ABasePlayer>(pController->GetPawn());
	if (pPlayer == nullptr) FinishTest(EFunctionalTestResult::Error, "pPlayer was null");

	pOxygenComp = pPlayer->GetOxygenComponent();
	if (pOxygenComp == nullptr) FinishTest(EFunctionalTestResult::Error, "pOxygenComp was null");

}

void AOxygenDeplenishFunctionalTest::StartTest()
{
	Super::StartTest();

	LogMessage(TEXT("[OxygenDeplenishFunctionalTest] Start Test"));

	pOxygenComp->SetMaxValue(fMaxOxygen);
	pOxygenComp->SetValue   (fMaxOxygen);
}

void AOxygenDeplenishFunctionalTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	pPlayer->DrainOxygen(fDeplenishRate * DeltaSeconds);

	if (pOxygenComp->IsEmpty())
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The component is now empty in " + FString::SanitizeFloat(fTimeTaken) + " seconds.");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	if (fTimeTaken > (fMaxOxygen / fDeplenishRate))
	{
		LogMessage("Expected: " + FString::SanitizeFloat(fMaxOxygen / fDeplenishRate) + " Actual: " + FString::SanitizeFloat(fTimeTaken));
		FinishTest(EFunctionalTestResult::Failed, "Took longer than expected to drain oxygen");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
	
	fTimeTaken += DeltaSeconds;

}