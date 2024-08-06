// Copyright Quarantine Games 2020

#include "../HealthDrainToReviveState_FTest.h"
#include "StellarFever/Player/SF_PlayerController.h"
#include "StellarFever/Player/BasePlayer.h"
#include "StellarFever/Components/HealthComponent.h"
#include "Engine/World.h"

void AHealthDrainToReviveState_FTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[HealthDrainToRevive] Prepare Test"));

	ASF_PlayerController* pc = Cast<ASF_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc == nullptr) FinishTest(EFunctionalTestResult::Error, "player controller was null");

	pPlayer = Cast<ABasePlayer>(pc->GetPawn());
	if (pPlayer == nullptr) FinishTest(EFunctionalTestResult::Error, "pPlayer was null");

	pHealthComp = pPlayer->GetHealthComponent();
	if (pHealthComp == nullptr) FinishTest(EFunctionalTestResult::Error, "pHealthcomp was null");

	fTimer = 0.0f;
}

void AHealthDrainToReviveState_FTest::Tick(float DeltaSeconds)
{
	float drainSpeed = pHealthComp->DefaultHealth / 10.0f;

	pPlayer->DrainHealth(DeltaSeconds * drainSpeed);

	if (pPlayer->GetIsPlayerDowned() && fTimer < 10.8f) //A 0.8 second buffer 
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The player was downed in the time we expected");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
	else if (fTimer > 10.08f)
	{
		FinishTest(EFunctionalTestResult::Failed, "The player was not downed in the time we expected");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	fTimer += DeltaSeconds;

}