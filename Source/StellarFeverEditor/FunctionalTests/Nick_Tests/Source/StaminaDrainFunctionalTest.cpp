// Copyright Quarantine Games 2020


#include "../StaminaDrainFunctionalTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Components/StatusComponent.h"
#include "Engine/World.h"


void AStaminaDrainFunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[StaminaDrainTest] Prepare Test"));

	pController  = Cast<ASF_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (pController == nullptr)  FinishTest(EFunctionalTestResult::Error, "pController was null");

	pPlayer      = Cast<ABasePlayer>(pController->GetPawn());
	if (pPlayer == nullptr)      FinishTest(EFunctionalTestResult::Error, "pPlayer was null");

	pStaminaComp = pPlayer->GetStaminaComponent();
	if (pStaminaComp == nullptr) FinishTest(EFunctionalTestResult::Error, "pStaminaComp was null");

}

void AStaminaDrainFunctionalTest::StartTest()
{
	Super::StartTest();
	LogMessage(TEXT("[StaminaDrainTest] Start Test"));

	//Set the value to zero since we are testing to replenish
	fMaxStamina = pStaminaComp->GetMaxValue();
	fDrainRate  = pPlayer->GetPlayerInformation().fStaminaDrainRate;

	pStaminaComp->SetMaxValue(fMaxStamina);
	pStaminaComp->SetValue(fMaxStamina);

}

void AStaminaDrainFunctionalTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	pPlayer->DrainStamina(fDrainRate * DeltaSeconds);

	if (pStaminaComp->IsEmpty())
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The stamina is now empty in " + FString::SanitizeFloat(fTimeTaken) + " seconds.");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	//Give it a 1 second buffer
	if (fTimeTaken > (fMaxStamina / fDrainRate) + 1.f)
	{
		LogMessage("Expected: " + FString::SanitizeFloat(fMaxStamina / fDrainRate) + " Actual: " + FString::SanitizeFloat(fTimeTaken));
		FinishTest(EFunctionalTestResult::Failed, "Took longer than expected to fill Stamina");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	fTimeTaken += DeltaSeconds;

}