// Fill out your copyright notice in the Description page of Project Settings.


#include "EventsManager.h"
#include "BaseEvent.h"
#include "EnemyPool.h"
#include "../StellarFeverGameModeBase.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "LevelProgression/RoundManager.h"


void UEventsManager::AddEventsToArray()
{
    //Searches the world for all events and adds them to the appropriate array.
    for (TActorIterator<ABaseEvent> ActorItr(pGameMode->GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->ActorHasTag("MainEvent"))
        {
            ABaseEvent* pTempEvent = Cast<ABaseEvent>(*ActorItr);
            pMainEvents.Add(pTempEvent);
            pTempEvent->SetEventsManager(this);
        }

        if (ActorItr->ActorHasTag("MinorEvent"))
        {
            ABaseEvent* pTempEvent = Cast<ABaseEvent>(*ActorItr);
            pMinorEvents.Add(pTempEvent);
            pTempEvent->SetEventsManager(this);
        }
    }
}

void UEventsManager::ResetAllEvents()
{
    //Resetting all the events to ensure they are inactive once the new round starts.
    for (int i = 0; i < pMainEvents.Num(); i++)
    {
        pMainEvents[i]->Deactivate();
        pMainEvents[i]->ResetActivation();
    }
    for (int i = 0; i < pMinorEvents.Num(); i++)
    {
        pMinorEvents[i]->Deactivate();
    }
}

void UEventsManager::SetUpEvents()
{
    //MIGHT BE USELESS

    //Used for basic set up on events, right now only applies player values in the player array
    for (int i = 0; i < pMainEvents.Num(); i++)
    {
        pMainEvents[i]->SetupPlayerPointer();
    }

    for (int i = 0; i < pMinorEvents.Num(); i++)
    {
        pMinorEvents[i]->SetupPlayerPointer();
    }
}

void UEventsManager::SetUpPools()
{
    //Finding and setting up the pool
    for (TActorIterator<AEnemyPool> ActorItr(pGameMode->GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->ActorHasTag("AIPool"))
        {
            AEnemyPool* tempPool = Cast<AEnemyPool>(*ActorItr);
            pEnemyPool = tempPool;
            pEnemyPool->SetEventsManager(this);
        }
    }
}

void UEventsManager::StartGlobalTimer()
{
    //Calls the Gamemode to unpause the level timer
    pGameMode->StartRoundTimer();
}

void UEventsManager::StopGlobalTimer()
{
    //Calls the Gamemode to pause the level timer
    pGameMode->StopRoundTimer();
}

void UEventsManager::SetEngineIsActive(bool isactive)
{
    //Passes in if the engine was active or not
    pGameMode->SetEngineIsActive(isactive);
}

void UEventsManager::ActivateMainEvent()
{
    //Randomly assigns which event will become active, will repeat the function if the random event is already active.

    int iRandomEvent = FMath::RandRange(0, pMainEvents.Num() - 1);
    ABaseEvent* pTempPointer;

    if (pMainEvents.IsValidIndex(iRandomEvent))
    {
        pTempPointer = Cast<ABaseEvent>(pMainEvents[iRandomEvent]);

        if (pMainEvents[iRandomEvent]->GetHasActivated() == true)
        {
            fRandomMainVal = FMath::FRandRange(1.0f, 3.0f);
            pGameMode->GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &UEventsManager::ActivateMainEvent, fRandomMainVal);
        }
        else
        {
            pMainEvents[iRandomEvent]->Activate();
            pRoundManager->SetNextEvent(pMainEvents[iRandomEvent]);
            fRandomMainVal = CreateDifficultyValue();
            pGameMode->GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &UEventsManager::ActivateMainEvent, fRandomMainVal);
        }
    }
}

void UEventsManager::ActivateMinorEvent()
{
    //Randomly assigns which minor event will become active, will repeat if the random event is already active
	
	//Leave if there are no events
	if (pMinorEvents.IsValidIndex(0) == false)
		return;

    int iRandomEvent = FMath::RandRange(0, pMinorEvents.Num() - 1);
    ABaseEvent* pTempPointer;
    pTempPointer = Cast<ABaseEvent>(pMinorEvents[iRandomEvent]);

    if (pTempPointer->GetIsActive() == true)
    {
        fRandomMinorVal = FMath::FRandRange(1.0f, 3.0f);
        pGameMode->GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &UEventsManager::ActivateMinorEvent, fRandomMinorVal);
    }
    else
    {
        pTempPointer->Activate();
        fRandomMinorVal = CreateMinorDifficultyValue();
        pGameMode->GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &UEventsManager::ActivateMinorEvent, fRandomMinorVal);
    }
}

void UEventsManager::TriggerMainEvent()
{
    //Searches through all the main events and finds one that is not active, if all are active it will exit

    //Leave if there are no events
    if (pMainEvents.IsValidIndex(0) == false)
        return;

    int iRandomEvent = FMath::RandRange(0, pMainEvents.Num() - 1);
    ABaseEvent* pTempPointer;
    pTempPointer = Cast<ABaseEvent>(pMainEvents[iRandomEvent]);

    if (pTempPointer->GetHasActivated() == true)
    {
        float fRandTimer = FMath::FRandRange(1.0f, 3.0f);
        pGameMode->GetWorld()->GetTimerManager().SetTimer(TriggerEventTimer, this, &UEventsManager::TriggerMainEvent, fRandTimer);
    }
    else
    {
        pTempPointer->Activate();
        pRoundManager->SetNextEvent(pMainEvents[iRandomEvent]);
    }


}

void UEventsManager::TriggerMinorEvent()
{
    //Will activate a random event on the minor event array, if it doesnt trigger an event it will search again in 5 seconds.

    //Leave if there are no events
    if (pMinorEvents.IsValidIndex(0) == false)
        return;

    int iRandomEvent = FMath::RandRange(0, pMinorEvents.Num() - 1);
    ABaseEvent* pTempPointer;
    pTempPointer = Cast<ABaseEvent>(pMinorEvents[iRandomEvent]);

    if (pTempPointer->GetIsActive() == true)
    {
        float fRandTimer = FMath::FRandRange(1.0f, 3.0f);
        pGameMode->GetWorld()->GetTimerManager().SetTimer(TriggerEventTimer, this, &UEventsManager::TriggerMinorEvent, fRandTimer);
    }
    else
    {
        pTempPointer->Activate();
    }


}

void UEventsManager::BlockInteraction(ABaseEvent* safeEvent)
{

}

void UEventsManager::AllowInteraction()
{

}

float UEventsManager::CreateDifficultyValue()
{
    //Calls the random value between the min and max, and add the difficulty scale afetrwards and returns the value
    float ftempValue = FMath::FRandRange(fRandomMinVal, fRandomMaxVal);
    ftempValue += fDifficultyScale * (4.0f / fDifficultyValue);

    return ftempValue;
}

float UEventsManager::CreateMinorDifficultyValue()
{
    //Calls the random value for minor events between min and max, adds the difficulty scale and returns the value
    float ftempValue = FMath::FRandRange(fRandomMinorMinVal, fRandomMinorMaxVal);
    ftempValue += fDifficultyMinorScale * (4.0f / fDifficultyValue);

    return ftempValue;
}

ABaseEvent* UEventsManager::GetEvent(FName eventName)
{
    //Looking through all main events to find a matching string
    for (int i = 0; i < pMainEvents.Num(); i++)
    {
        if (pMainEvents[i]->ActorHasTag(eventName))
        {
            return pMainEvents[i];
        }
    }

    //If no event matched the tag will return null
    return nullptr;
}

bool UEventsManager::CheckIfEventsAreActive()
{
    //Two quick loops going through all the events and seeing if any are active, if even one is it will return false, else it will return true
    for (int i = 0; i < pMainEvents.Num(); i++)
    {
        if (pMainEvents[i]->GetIsActive() == true)
        {
            return true;
        }
    }

    for (int i = 0; i < pMinorEvents.Num(); i++)
    {
        if (pMinorEvents[i]->GetIsActive() == true)
        {
            return true;
        }
    }
    return false;
}

void UEventsManager::StopEventTimers()
{
    //Clears and stops all timers
    pGameMode->GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);
    pGameMode->GetWorld()->GetTimerManager().ClearTimer(MinorEventTimer);
}

void UEventsManager::StartEventTimers()
{
    //Setting up timers and the random values determined in blueprints, the timers are meant to be reset whenever events are reset as well for new round.
    fRandomMainVal = CreateDifficultyValue();
    fRandomMinorVal = CreateMinorDifficultyValue();

    pGameMode->GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &UEventsManager::ActivateMainEvent, fRandomMainVal);
    pGameMode->GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &UEventsManager::ActivateMinorEvent, fRandomMinorVal);
}
