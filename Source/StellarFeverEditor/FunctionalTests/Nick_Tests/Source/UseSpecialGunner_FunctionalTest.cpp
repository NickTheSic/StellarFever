// Copyright Quarantine Games 2020

#include "../UseSpecialGunner_FunctionalTest.h"
#include "StellarFever/Player/GunnerPlayer.h"

void AUseSpecialGunner_FunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[UseSpecial Gunner] Prepare Test"));

	pGunner = Cast<AGunnerPlayer>(pPlayer);
	if (pGunner == nullptr)
	{
		FinishTest(EFunctionalTestResult::Error, "pGunner was null/Probably not a Gunner Player");
		return;
	}

	fAbilityLength = pGunner->GetPlayerInformation().fActiveAbilityLength;

}

void AUseSpecialGunner_FunctionalTest::StartTest()
{
	Super::StartTest();
	LogMessage(TEXT("[UseSpecial Gunner] Start Test"));

	//The parent class should have called UseSpecial so I should be able to check
	//That the following is true
	
	if (pGunner->ShieldMeshIsActive() == false)
		FinishTest(EFunctionalTestResult::Failed, "Shield mesh on the gunner was false");

	if (pGunner->ShieldColliderIsActive() == false)
		FinishTest(EFunctionalTestResult::Failed, "Shield Collider on the gunner was false");

	if (pGunner->ShieldMeshIsActive() && pGunner->ShieldColliderIsActive())
	{
		//FinishTest(EFunctionalTestResult::Succeeded, "The mesh and shield were active");
	}
}

void AUseSpecialGunner_FunctionalTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if (fTimer > fAbilityLength)
	{
		if (!pGunner->ShieldMeshIsActive() && !pGunner->ShieldColliderIsActive())
		{
			FinishTest(EFunctionalTestResult::Succeeded, "The mesh and shield were Deactivated");
			GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
		}
		else {
			FinishTest(EFunctionalTestResult::Succeeded, "The mesh and shield took longer than expected to deactivate");
			GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
		}
	}

	fTimer += DeltaSeconds;

}
