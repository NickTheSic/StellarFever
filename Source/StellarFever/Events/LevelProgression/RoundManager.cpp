// Copyright Quarantine Games 2020


#include "RoundManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "EngineUtils.h"
#include "../EventsManager.h"
#include "../../StellarFeverGameModeBase.h"
#include "../../Miscellaneous/ShipState.h"
#include "../EventObject.h"
#include "RoundPlanet.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "../BaseEvent.h"
#include "../ShipSystems/Shop/Shipshop.h"
#include "Net/UnrealNetwork.h"
#include "../../Tutorial/TutorialManager.h"
#include "../../Tutorial/TutorialManager2.h"
#include"../../Player/BasePlayer.h"
#include "../PickupItems/VaccineObject.h"

// Sets default values
ARoundManager::ARoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creating the mesh and colider objets
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pMesh->SetIsReplicated(true);
	RootComponent = pMesh;

	//Collider used to animate the planets spawnign in
	pCollider = CreateDefaultSubobject<UBoxComponent>("AnimCollider");
	pCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pCollider->SetupAttachment(RootComponent);

	pTimerText = CreateDefaultSubobject<UTextRenderComponent>("TimerText");
	pTimerText->SetIsReplicated(true);
	pTimerText->SetupAttachment(RootComponent);

	pFuelText = CreateDefaultSubobject<UTextRenderComponent>("FuelText");
	pFuelText->SetIsReplicated(true);
	pFuelText->SetupAttachment(RootComponent);

	pLevelSelectAudioManager = CreateDefaultSubobject<UAudioComponent>("LevelSelect");
	pLevelSelectAudioManager->SetIsReplicated(true);
	pLevelSelectAudioManager->SetupAttachment(RootComponent);

	pLevelBaseAudioManager = CreateDefaultSubobject<UAudioComponent>("LevelBase");
	pLevelBaseAudioManager->SetIsReplicated(true);
	pLevelBaseAudioManager->SetupAttachment(RootComponent);

	pNextEvent = nullptr;
	ucLevelValue = 0;

	Tags.Add("RoundManager");

	bAlwaysRelevant = true;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void ARoundManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		//Overlaps for the planets to reveal themselves
		pCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ARoundManager::OnColliderOverlap);
		pCollider->OnComponentBeginOverlap.AddDynamic(this, &ARoundManager::OnColliderOverlap);
		pCollider->OnComponentEndOverlap.RemoveDynamic(this, &ARoundManager::OnColliderExit);
		pCollider->OnComponentEndOverlap.AddDynamic(this, &ARoundManager::OnColliderExit);
	}

	//Loops through all added planets in the array in the blueprint to add the roundmanager in properly
	for (int i = 0; i < pPlanets.Num(); i++)
	{
		pPlanets[i]->SetRoundManager(this);
	}

	//Setting the shop up properly
	if (pShop != nullptr)
	{
		pShop->SetRoundManager(this);
	}

	//Setting the sounds to the correct audiomanager
	pLevelBaseAudioManager->SetSound(pLevelBaseMusic);
	pLevelSelectAudioManager->SetSound(pLevelSelectMusic);
}

// Called every frame
void ARoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoundManager::StartLevel(ARoundPlanet* planet)
{
	//A check if the game is in the tutorial or not
	if (bTutorial == true)
	{
		//for (TActorIterator<ATutorialManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		//{
		//	if (ActorItr)
		//	{
		//		if (ActorItr->GetBaysCheck() == false)
		//		{
		//			//On tutorial will set the engine event and display the similar ship broken text
		//			pEventManager->GetEvent(FName("Engine"))->Activate();
		//			pGamemode->SetRoundTime(1.0f);
		//			pGamemode->StartRoundTimer();
		//			pTimerText->SetText(FText::FromString("Ship still broken"));
		//			ActorItr->ShutDownEngines();
		//		}
		//		else
		//		{
		//			pTimerText->SetText(FText::FromString(""));
		//			ActorItr->PlayFarewell();
		//		}
		//	}
		//}
		for (TActorIterator<ATutorialManager2> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr)
			{
				//activate test event
				pEventManager->GetEvent(FName("Test Event"))->Activate();
				ActorItr->PlayFixEvent();
				ActorItr->IncrementProgression();
			}
		}
	}
	else
	{
		Server_StartLevel(planet);
	}
}

void ARoundManager::Server_StartLevel_Implementation(ARoundPlanet* planet)
{
	ucPlayersTouching = 0;
	//A quick check applying to the frist time a level is started to add all players into an array
	if (ucLevelValue == 0)
	{
		AddPlayersToArray();
	}

	//Grabbing all variables from the planet to be used
	float roundtime = planet->GetRoundTime();
	float difficultyvalue = planet->GetDifficultyValue();
	FName eventname = planet->GetEventName();

	//Applying all the values properly to gamemode
	pGamemode->SetRoundTime(roundtime);
	pGamemode->StartRoundTimer();
	SetTimerText(roundtime);

	//Setting the difficulty, if set to 0 in the planet it will not increase
	SetDifficultyValue(fDifficultyValue + difficultyvalue);

	//Activating the proper event
	pEventManager->ResetAllEvents();
	pEventManager->GetEvent(eventname)->Activate();
	pNextEvent = pEventManager->GetEvent(eventname);
	pEventManager->StartEventTimers();

	//Increase the value of how manhy levels have been done
	ucLevelValue++;

	//Finds a vaccine on the ship and DELETES it from the game
	for (TActorIterator<AVaccineObject> ActorItr(pGamemode->GetWorld()); ActorItr; ++ActorItr)
	{
		AVaccineObject* ptempvaccine = Cast<AVaccineObject>(*ActorItr);
		if (ptempvaccine->GetIsActive() == true)
		{
			ptempvaccine->Destroy();
			pGamemode->GetShipState()->SubVaccines(1);
		}
		break;
	}
}

void ARoundManager::EventsActiveCheck()
{
	//a check if we are in tutorial
	if (bTutorial == true)
	{
		GetWorld()->GetTimerManager().SetTimer(EventCheckTimer, this, &ARoundManager::EventsActiveCheck, fEventCheckTime);
		//TODO: Add in the tutorial manager and ask for a call when the rest of the map has been interacted with
		/*
		for (TActorIterator<ATutorialManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr)
			{
				//If engines are working and we've already activated engines
				if (pEventManager->GetEvent(FName("Engine"))->bIsActive == false && 
					ActorItr->GetEngineCheck() == true &&
					ActorItr->GetHPOxygenCheck() == false)
				{
					//Activate LifeSupport and OxygenSupply
					pEventManager->GetEvent(FName("LifeSupport"))->Activate();
					pEventManager->GetEvent(FName("OxygenSupply"))->Activate();
					//Spawn enemies who "shut down" the ship
					ActorItr->ShutDownHealthOxygen();
				}
				//If we fixed lifesupport and oxygen, and we've finished the engines as well, tell player to get health and ammo
				if (pEventManager->GetEvent(FName("LifeSupport"))->bIsActive == false &&
					pEventManager->GetEvent(FName("OxygenSupply"))->bIsActive == false &&
					ActorItr->GetEngineCheck() == true &&
					ActorItr->GetHPOxygenCheck() == true &&
					ActorItr->GetArmoryCheck() == false)
				{
					//start doing armory things
					ActorItr->UnlockArmory();
				}
				if (ActorItr->GetBaysCheck())
				{
					Activate();
				}
			}
		}*/

		for (TActorIterator<ATutorialManager2> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr)
			{
				//if test event is no longer activated and we are at progression 2
				if (pEventManager->GetEvent(FName("Test Event"))->bIsActive == false &&
					ActorItr->GetProgression() == 2)
				{
					//we fixed the event and can move on
					ActorItr->IncrementProgression();
					ActorItr->PlayEventLine();
				}
			}
		}
	}
	else
	{
		//A check to ensure if any events are truly activated and start up the next round
		//Called by the game mode once the tiemr reaches 0
		if (GetLocalRole() == ROLE_Authority)
		{
			if (pEventManager->CheckIfEventsAreActive() == true)
			{
				GetWorld()->GetTimerManager().SetTimer(EventCheckTimer, this, &ARoundManager::EventsActiveCheck, fEventCheckTime);
			}
			if (pEventManager->CheckIfEventsAreActive() == false)
			{
				Reset();
			}
		}
	}
}

ARoundPlanet* ARoundManager::GetRandomPlanet()
{
	//loops through the list of the planets and grabs one
	int randomvalue = FMath::RandRange(0, pPlanets.Num() - 1);
	ARoundPlanet* tempPointer;
	tempPointer = pPlanets[randomvalue];

	//specific part of loop that just increments by one when enable to be used, and returns to 0 when too great a value
	while (tempPointer->GetisUsed() == true || tempPointer->GetIsActive() == true)
	{
		randomvalue++;
		if (randomvalue > pPlanets.Num() - 1)
		{
			randomvalue = 0;
		}
		tempPointer = pPlanets[randomvalue];
	}
	return tempPointer;
}

void ARoundManager::Deactivate()
{
	Server_Deactivate();

	Multicast_PlaySound(false);
}

void ARoundManager::Server_Deactivate_Implementation()
{
	//Ensures the proper text is displayed
	pTimerText->SetVisibility(true);
	bPlayerCheck = false;

	if (!bTutorial)
	{
		//Safety to not cause double level checks? Bugs do arise cause of this timer
		GetWorld()->GetTimerManager().ClearTimer(EventCheckTimer);
	}

	//Removes all planets from the array to clear space for new planets to be placed in
	for (int i = 0; i < ucNumOfChoices; i++)
	{
		//pNextRounds[i]->pPlanetWidget->
		pNextRounds[i]->Disable();
	}

	//Deactivates the shop if it exists
	if (pShop != nullptr)
	{
		pShop->Deactivate();
	}

	//Clears the list of planets
	pNextRounds.Empty();

	//Sets the bool to false
	bIsActive = false;
}

void ARoundManager::Activate()
{
	Server_Activate();

	Multicast_PlaySound(true);
}

void ARoundManager::Server_Activate_Implementation()
{
	//Ensures the proper text is displayed
	pTimerText->SetVisibility(false);

	//Safety to not cause double level checks? Bugs do arise cause of this timer
	GetWorld()->GetTimerManager().ClearTimer(EventCheckTimer);

	//Activates the planets to be used, will use 2 random ones generated from the RandomPlanet function
	const float leftOffset = ((ucNumOfChoices - 1) / 2.f) * -175.f;
	for (int i = 0; i < ucNumOfChoices; i++)
	{
		pNextRounds.Add(GetRandomPlanet());			
		//pNextRounds[i]->SetActorLocation(GetActorLocation() + FVector(0.0f, -100 + (i * 100), 150.0f));
		pNextRounds[i]->SetActorLocation(GetActorLocation() + FVector(0.0f, leftOffset + (i * 175.f), 150.0f));
		pNextRounds[i]->Activate();
		pNextRounds[i]->DeactivateInfo();
	}

	//Activates the shop if it exists
	if (pShop != nullptr && ucLevelValue > 0)
	{
		pShop->Activate();
	}

	//Sets the bool to true
	bIsActive = true;
}

void ARoundManager::Reset()
{
	//Basic init per round
	Activate();

	//resets events to be activated again
	if (pEventManager != nullptr)
	{
		pEventManager->ResetAllEvents();
	}
}

void ARoundManager::SetTimerText(float roundTime)
{
	Multicast_SetTimerText(roundTime);
}

void ARoundManager::Multicast_SetTimerText_Implementation(float roundTime)
{
	if (roundTime < 0)
	{
		if (pGamemode != nullptr && pGamemode->GetShipState() != nullptr)
		{
			if (pGamemode->GetShipState()->GetVaccines() > 0)
			{
				//When timer is below 0 and event is still active.
				pTimerText->SetText(FText::FromString("Ship still broken"));

				//A check if all players are on the proper ship before jumping
				for (int i = 0; i < pPlayers.Num(); i++)
				{
					if (pPlayers[i]->GetIsOnEnemyShip() == true)
					{
						pTimerText->SetText(FText::FromString("Not all players on ship"));
					}
				}
			}
			else
			{
				//Tells the player we need a vaccine
				pTimerText->SetText(FText::FromString("No Vaccines!"));
			}
		}
	}
	else
	{
		//Sets the text to be the passed in value
		pTimerText->SetText(FText::FromString("Distance Remaining: " + FString::SanitizeFloat(roundTime) + " Galactometers"));
	}
}

void ARoundManager::SetFuelText(float fuel)
{
	Multicast_SetFuelText(fuel);
}

void ARoundManager::Multicast_SetFuelText_Implementation(float fuel)
{
	//Because the function is called everytime fuel or roundtime decreases i placed it with the set timer text
	pFuelText->SetText(FText::FromString(FString::SanitizeFloat(fuel) + " Spaceliters of SPS Branded Fuel"));
}

void ARoundManager::SetDifficultyValue(float difficultyValue)
{
	//roundmanager changes the difficulty value each level change so to be sure, it will change all potential difficulty values as well.
	fDifficultyValue = difficultyValue;
	pGamemode->SetDifficultyValue(difficultyValue);
	pEventManager->SetDifficultyValue(difficultyValue);
}

void ARoundManager::PlayEventMusic()
{
	//Gets the event to play it's specific audio when the track ends
	if (pNextEvent != nullptr)
	{
		pNextEvent->Multicast_PlayMusic();
		pNextEvent = nullptr;
		GetWorld()->GetTimerManager().SetTimer(DurationTimer, this, &ARoundManager::PlayEventMusic, fClipDuration, true);
	}
}

void ARoundManager::AddPlayersToArray()
{
	//Loops through al players and adds them to the array
	for (TActorIterator<ABasePlayer> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ABasePlayer* pTempplayer = Cast<ABasePlayer>(*ActorItr);
		pPlayers.Add(pTempplayer);
	}
}

void ARoundManager::Multicast_PlaySound_Implementation(bool blevelselect)
{
	//Plays the audio for the level select
	if (blevelselect == true && pLevelSelectMusic != nullptr)
	{
		if (pLevelBaseAudioManager->IsPlaying() == true)
		{
			pLevelBaseAudioManager->FadeOut(5.0f, 0.0f);
		}
		pLevelSelectAudioManager->FadeIn(25.0f);
		GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
	}

	//Plays the regular audio for mid game
	if (blevelselect == false && pLevelBaseMusic != nullptr)
	{
		if (pLevelSelectAudioManager->IsPlaying() == true)
		{
			pLevelSelectAudioManager->FadeOut(5.0f, 0.0f);
		}
		pLevelBaseAudioManager->FadeIn(25.0f);
		GetWorld()->GetTimerManager().SetTimer(DurationTimer, this, &ARoundManager::PlayEventMusic, fClipDuration, true);
	}

}

void ARoundManager::CheckEvents()
{
	//A check to ensure if any events are truly activated and start up the next round
	//Called by the game mode once the tiemr reaches 0
	if (GetLocalRole() == ROLE_Authority && bIsActive == false)
	{
		if (pEventManager->CheckIfEventsAreActive() == true)
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(EventTriggerTimer))
			{
				GetWorld()->GetTimerManager().ClearTimer(EventTriggerTimer);
			}
			return;
		}
		if (pEventManager->CheckIfEventsAreActive() == false && GetWorld()->GetTimerManager().IsTimerActive(EventTriggerTimer) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(EventTriggerTimer, this, &ARoundManager::ActivateEvent, 5.0f, false);
		}
	}
}

void ARoundManager::ActivateEvent()
{
	pEventManager->ActivateMainEvent();
}

void ARoundManager::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		if (bIsActive == true)
		{
			if (ucPlayersTouching == 0)
			{
				for (int i = 0; i < pNextRounds.Num(); i++)
				{
					if (pNextRounds[i] != nullptr)
					{
						pNextRounds[i]->SetupPlanetText();
						pNextRounds[i]->ActivateInfo();
					}
				}
				bPlayerCheck = true;
			}
			ucPlayersTouching = ucPlayersTouching == 4 ? 4 : ++ucPlayersTouching;
		}
	}
}

void ARoundManager::OnColliderExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		if (bIsActive == true)
		{
			if (ucPlayersTouching == 1)
			{
				for (int i = 0; i < pNextRounds.Num(); i++)
				{
					if (pNextRounds[i] != nullptr)
					{
						pNextRounds[i]->DeactivateInfo();
					}
				}
				bPlayerCheck = false;
			}
			ucPlayersTouching = ucPlayersTouching == 0 ? 0 : --ucPlayersTouching;
		}
	}
}

void ARoundManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARoundManager, ucLevelValue);
	DOREPLIFETIME(ARoundManager, pTimerText);
	DOREPLIFETIME(ARoundManager, pFuelText);
	DOREPLIFETIME(ARoundManager, fDifficultyValue);
	DOREPLIFETIME(ARoundManager, bIsActive);
	DOREPLIFETIME(ARoundManager, pPlanets);
	DOREPLIFETIME(ARoundManager, pNextRounds);
	DOREPLIFETIME(ARoundManager, bPlayerCheck);
	DOREPLIFETIME(ARoundManager, pShop);
	DOREPLIFETIME(ARoundManager, ucPlayersTouching);
}


