// Copyright Quarantine Games 2020


#include "../UseSpecialBase_FunctionalTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Player/BasePlayer.h"

void AUseSpecialBase_FunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[UseSpecial Base] Prepare Test"));

	pController = Cast<ASF_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (pController == nullptr)  FinishTest(EFunctionalTestResult::Error, "pController was null");

	pPlayer = Cast<ABasePlayer>(pController->GetPawn());
	if (pPlayer == nullptr)      FinishTest(EFunctionalTestResult::Error, "pPlayer was null");

}

void AUseSpecialBase_FunctionalTest::StartTest()
{
	Super::StartTest();
	LogMessage(TEXT("[UseSpecial Base] Start Test"));

	//Call the class special
	LogMessage(TEXT("Calling ClassSpecial on pController"));
	pController->ClassSpecial();

	if (pPlayer->GetSpecialIsActive() == false)
	{
		FinishTest(EFunctionalTestResult::Failed, "The player isn't returning a true SpecialIsActive");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	if (pPlayer->GetCanUseSpecial() == true)
	{
		FinishTest(EFunctionalTestResult::Failed, "The player can still use special even after 'using' it");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

}
