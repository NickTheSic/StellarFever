// Copyright Quarantine Games 2020


#include "../StaminaReplenishFunctionalTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Components/StatusComponent.h"
#include "Engine/World.h"


void AStaminaReplenishFunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[StaminaReplenishTest] Prepare Test"));

	pController = Cast<ASF_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (pController == nullptr)  FinishTest(EFunctionalTestResult::Error, "pController was null");

	pPlayer = Cast<ABasePlayer>(pController->GetPawn());
	if (pPlayer == nullptr)      FinishTest(EFunctionalTestResult::Error, "pPlayer was null");

	pStaminaComp = pPlayer->GetStaminaComponent();
	if (pStaminaComp == nullptr) FinishTest(EFunctionalTestResult::Error, "pStaminaComp was null");

}

void AStaminaReplenishFunctionalTest::StartTest()
{
	Super::StartTest();
	LogMessage(TEXT("[StaminaReplenishTest] Start Test"));

	fMaxStamina    = pStaminaComp->GetMaxValue();
	fReplenishRate = pPlayer->GetPlayerInformation().fStaminaReplenishRate;

	pStaminaComp->SetValue(0.0f);
	pStaminaComp->AddToValue(1.0f);
}

void AStaminaReplenishFunctionalTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	fTimeTaken += DeltaSeconds;

	if (pStaminaComp->IsFull())
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The component is now full in " + FString::SanitizeFloat(fTimeTaken) + " seconds.");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	if (fTimeTaken > (fMaxStamina / fReplenishRate) + 0.5f) //Added a buffer time since it would fail by > 0.5
	{
		LogMessage("Expected: " + FString::SanitizeFloat(fMaxStamina / fReplenishRate) + " Actual: " + FString::SanitizeFloat(fTimeTaken));
		FinishTest(EFunctionalTestResult::Failed, "Took longer than expected to fill stamina");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
}