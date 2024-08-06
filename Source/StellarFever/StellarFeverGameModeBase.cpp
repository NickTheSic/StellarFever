// Copyright Epic Games, Inc. All Rights Reserved.


#include "StellarFeverGameModeBase.h"
#include "StellarFeverGameState.h"
#include "AI/BaseAICharacter.h"
#include "AI/BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/Engine.h"
#include "Events/EventsManager.h"
#include "Events/LevelProgression/RoundManager.h"
#include "Events/ShipSystems/Shop/Shipshop.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BasePlayer.h"
#include "Player/SF_PlayerController.h"
#include "StellarFeverGameInstance.h"
#include "Miscellaneous/ShipState.h"

AStellarFeverGameModeBase::AStellarFeverGameModeBase()
{

}

void AStellarFeverGameModeBase::BeginPlay()
{
    PrimaryActorTick.bCanEverTick = true;
    GetWorldTimerManager().SetTimer(DeductBySecondTimer, this, &AStellarFeverGameModeBase::DeductSecond, 1.0f, true);

	//Once we reach the right number of players set up everything properly for the events
	pEventManager = NewObject<UEventsManager>(EventManagerClass, "EventManager");
	pEventManager->SetEventsGameMode(this);
	pEventManager->AddEventsToArray();
	pEventManager->ResetAllEvents();
	pEventManager->SetUpPools();

	//Setting the difficulty to the number of players
	fDifficultyValue = GetNumPlayers();
	pEventManager->SetDifficultyValue(ucPlayerVal);

	//Searches the world for the roundmanager, needs to be an active actor in the world
	for (TActorIterator<ARoundManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->ActorHasTag("RoundManager"))
		{
			pRoundManager = *ActorItr;
			break;
		}
	}

	//Getting the shipstate and assigning it to the proper variable
	AStellarFeverGameState* ptempstate = Cast<AStellarFeverGameState>(GetWorld()->GetGameState());
	pShipState = ptempstate->GetShipState();

	//Used to set up the roundmanager to activate in base state also checks so doesnt crash non round manager maps
	if (pRoundManager != nullptr)
	{
		pRoundManager->SetEventManager(pEventManager);
		pRoundManager->SetGameMode(this);
		pRoundManager->SetDifficultyValue(fDifficultyValue);
		pRoundManager->Reset();
		pEventManager->SetRoundManager(pRoundManager);
	}

	if (pShipState != nullptr)
	{
		pShipState->SetRoundManager(pRoundManager);
		//Sets the base value of the fuel
		pShipState->SetFuelAmount(fStartingFuelVal);
		//Sets the starting amount of vaccines
		pShipState->SetVaccines(5.0f);
		if (pRoundManager != nullptr)
		{
			if (pRoundManager->GetShop() != nullptr)
			{
				pRoundManager->GetShop()->SetShipState(pShipState);
			}
		}
	}
}

void AStellarFeverGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//Finding how many players there are
	if (GetGameInstance()->GetNumLocalPlayers() > ucPlayerVal)
	{
		ucPlayerVal++;
		if (GetGameInstance()->GetNumLocalPlayers() == ucPlayerVal)
		{
			//Keeping here, did not work properly :(
		}
	}

	//Getting all the logging in player controllers for the End Game
	if (ASF_PlayerController* sfPlayer = Cast<ASF_PlayerController>(NewPlayer))
	{
		PlayerControllerArray.Add(sfPlayer);
	}

}

void AStellarFeverGameModeBase::SetEngineIsActive(bool isactive)
{
	//A base check to see if the variable passed in is true of false
	if (isactive == true)
	{
		//Sets the proper variable, normal isengine check is for 1 engine, both is for 2
		if (bIsEngineActive == true)
		{
			bIsBothEngineActive = true;
		}
		else
		{
			bIsEngineActive = true;
		}
	}

	//Because events call deactivate when they are spawned, this first if makes sure that they were once active beforehand
	if (bIsEngineActive == true)
	{
		if (isactive == false)
		{
			//Does the opposite of the ==true variant above, check if both are active first then deducting the right one
			if (bIsBothEngineActive == true)
			{
				bIsBothEngineActive = false;
			}
			else
			{
				bIsEngineActive = false;
			}
		}
	}
}

void AStellarFeverGameModeBase::DeductSecond()
{
	if (pShipState == nullptr || pRoundManager == nullptr || pEventManager == nullptr)
		return;

	//The initial chekc if we even have fuel to proceed, timer stops without fuel
	float tempval = pShipState->GetFuelAmount();
	if (tempval > 0.0f)
	{
		//If the bridge isnt broken time will deduct properly
		if (bIsBridgeActive == false)
		{
			fRoundTime--;
			pRoundManager->CheckEvents();
		}

		//Lowers the fuel count by 1
		float subval = 0.0f;
		if (tempval > 0.0f)
		{
			subval++;

			//If any engine is active, the fuel lowers twice as fast
			if (bIsEngineActive == true)
			{
				subval++;
			}
		}

		//Reassigns the ships fuel amount to the new one
		pShipState->SubFuelAmount(subval);

		if (pRoundManager != nullptr)
		{
			pRoundManager->SetTimerText(fRoundTime);
			if (fRoundTime < 0.0f)
			{
				if (pShipState->GetVaccines() > 0)
				{
					//Stops the events from activating any more and will set up the round timer check
					pEventManager->StopEventTimers();
					pRoundManager->EventsActiveCheck();
					GetWorldTimerManager().PauseTimer(DeductBySecondTimer);

					//A Check to see that the players made 5 jumps
					if (pRoundManager->GetNumberOfLevels() == 5)
					{
						EndGame(EEndGameReason::GameWon);
						//UGameplayStatics::OpenLevel(this, "MainMenuMap");
					}
				}
			}
		}
	}
}

void AStellarFeverGameModeBase::StartRoundTimer()
{
	bIsBridgeActive = false;
	if (GetWorldTimerManager().IsTimerPaused(DeductBySecondTimer) == true)
	{
		GetWorldTimerManager().UnPauseTimer(DeductBySecondTimer);
	}
}

void AStellarFeverGameModeBase::StopRoundTimer()
{
	bIsBridgeActive = true;
}

void AStellarFeverGameModeBase::WhenPlayersAllDie()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::SanitizeFloat(PlayerAmount));

	if (PlayerAmount > 0) return; //Return if we have more than 0 players 
								  //to avoid Searching for all enemies

	EndGame(EEndGameReason::AllPlayersDied);
}

void AStellarFeverGameModeBase::EndGame(const EEndGameReason& EndGameReason)
{
	for (ASF_PlayerController* controller : PlayerControllerArray)
	{
		controller->TriggerEndGame(EndGameReason);
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAICharacter::StaticClass(), AllEnemies);

	for (int i = 0; i < AllEnemies.Num() - 1; i++)
	{
		ABaseAICharacter* enemy = Cast<ABaseAICharacter>(AllEnemies[i]);
		if (enemy)
		{
			ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(enemy->GetController());
			if (controller)
			{
				UObject* mechanismObject = controller->GetBlackboardComponent()->GetValueAsObject("TargetMechanism");
				AActor* mechanismActor = Cast<AActor>(mechanismObject);

				enemy->bDontFire = true;
				enemy->bEndGame = true;
				//controller->GetBlackboardComponent()->SetValueAsObject("TargetActor", mechanismObject); //change so enemy goes towards end goal
				//UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, mechanismActor->GetActorLocation());
				//controller->MoveToActor(mechanismActor);
			}
		}
	}
}

