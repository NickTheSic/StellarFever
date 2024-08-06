// Copyright Quarantine Games 2020


#include "../UseSpecialEngineerFunctionalTest.h"
#include "StellarFever/Player/EngineerPlayer.h"

void AUseSpecialEngineerFunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[UseSpecial Gunner] Prepare Test"));

	pEngineer = Cast<AEngineerPlayer>(pPlayer);
	if (pEngineer == nullptr) FinishTest(EFunctionalTestResult::Error, "pEngineer was null/Probably not an Engineer Player");

}

void AUseSpecialEngineerFunctionalTest::StartTest()
{
	Super::StartTest();
	LogMessage(TEXT("[UseSpecial Gunner] Start Test"));

	//The parent class should have called UseSpecial so I should be able to check
	//That the following is true

	//Since his special is instant this is the easiest way to check with what is currently implemented
	if (pEngineer->GetCanUseSpecial() == false)
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The engineer used his special");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
}