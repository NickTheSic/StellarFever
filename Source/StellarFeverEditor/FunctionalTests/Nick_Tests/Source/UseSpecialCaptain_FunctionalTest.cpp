// Copyright Quarantine Games 2020

#include "../UseSpecialCaptain_FunctionalTest.h"
#include "StellarFever/Player/CaptainPlayer.h"
#include "Kismet/GameplayStatics.h"

void AUseSpecialCaptain_FunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	LogMessage(TEXT("[UseSpecial Captain] Prepare Test"));

	pCaptain = Cast<ACaptainPlayer>(pPlayer);
	if (pCaptain == nullptr)
	{
		FinishTest(EFunctionalTestResult::Error, "pCaptain was null/Probably not a Captain Player");
		return;
	}
	startingLocation = pCaptain->GetActorLocation();

	TArray<AActor*> targetSpots;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Teleportable", targetSpots);

	if (targetSpots.Num() == 0)
	{
		FinishTest(EFunctionalTestResult::Error, "No target spots set in this level");
		return;
	}

	//Set the ending location and teleport index to the same actor
	endingLocation = targetSpots[0]->GetActorLocation();
	pCaptain->SetTeleportInformation(0);
}

void AUseSpecialCaptain_FunctionalTest::StartTest()
{
	Super::StartTest();

	//The Super class calls ClassSpecial so I am hoping we can just check for everything here
	if (pCaptain->GetActorLocation() == endingLocation) //TODO: Change it to a range, since we are failing
	{
		FinishTest(EFunctionalTestResult::Succeeded, "The captain was at the end location");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
	else if (pCaptain->GetActorLocation() == startingLocation)
	{
		FinishTest(EFunctionalTestResult::Failed, "The captain is still at the Starting Location");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
	else
	{
		FinishTest(EFunctionalTestResult::Failed, "The captain was sent to the wrong location");
		GetWorld()->GetFirstPlayerController()->ConsoleCommand(TEXT("Exit"), true);
	}
}