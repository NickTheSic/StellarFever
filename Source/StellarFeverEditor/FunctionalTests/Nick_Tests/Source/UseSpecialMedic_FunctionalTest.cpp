// Copyright Quarantine Games 2020


#include "../UseSpecialMedic_FunctionalTest.h"
#include "StellarFever/Player/MedicPlayer.h"
#include "StellarFever/Components/HealthComponent.h"

void AUseSpecialMedic_FunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[UseSpecial Medic] Prepare Test"));

	pMedic = Cast<AMedicPlayer>(pPlayer);
	if (pMedic == nullptr)
	{
		FinishTest(EFunctionalTestResult::Error, "pMedic was null/Probably not a pMedic Player");
		return;
	}

	pHealthComp = pMedic->GetHealthComponent();
	if (pHealthComp == nullptr) FinishTest(EFunctionalTestResult::Error, "The health component was null");

	//Set the default health to a low number so we can heal
	pHealthComp->Health = 1.0f;
	fHealRate = pMedic->GetMedicHealSpeed();
	fHealTime = pMedic->GetPlayerInformation().fActiveAbilityLength;

}

void AUseSpecialMedic_FunctionalTest::StartTest()
{
	Super::StartTest();
	LogMessage(TEXT("[UseSpecial Medic] Start Test"));
}

void AUseSpecialMedic_FunctionalTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (pHealthComp->GetHealth() >= (fHealRate * fHealTime) && fTimer <= fHealTime)
	{
		FinishTest(EFunctionalTestResult::Succeeded, "We healed the expected amount in the rigth amount of time");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	if (fTimer > fHealTime + 0.5f){
		FinishTest(EFunctionalTestResult::Failed, "Took longer than expected to heal");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}

	fTimer += DeltaSeconds;

}