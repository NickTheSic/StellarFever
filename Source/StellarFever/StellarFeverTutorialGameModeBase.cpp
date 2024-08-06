// Copyright Quarantine Games 2020


#include "StellarFeverTutorialGameModeBase.h"
#include "Components/HealthComponent.h"
#include "Events/BaseEvent.h"
#include "Events/ShipSystems/DoorObject.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BasePlayer.h"
#include "SFBasicSaveGame.h"
#include "StellarFeverGameInstance.h"
#include "TimerManager.h"
#include "Events/MainEvents/BoardingEvent.h"

void AStellarFeverTutorialGameModeBase::BeginPlay()
{
    for (TActorIterator<ABaseEvent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->ActorHasTag("MainEvent"))
        {
            if (ActorItr->ActorHasTag("Boarding"))
            {
                continue;
            }
            else
            {
                ABaseEvent* pTempEvent = Cast<ABaseEvent>(*ActorItr);
                pMainEvents.Add(pTempEvent);
                pTempEvent->SetEventsManager(nullptr);
            }
        }
    }

    for (TActorIterator<ADoorObject> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->ActorHasTag("Door1"))
        {
            ADoorObject* pTempEvent = Cast<ADoorObject>(*ActorItr);
            pDoor1 = pTempEvent;
            pDoor1->SetDoorToBroken();
            bDoor1isOpen = false;           
        }
    }

    for (TActorIterator<ADoorObject> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->ActorHasTag("Door2"))
        {
            ADoorObject* pTempEvent = Cast<ADoorObject>(*ActorItr);
            pDoor2 = pTempEvent;
            pDoor2->SetDoorToBroken();
            bDoor2isOpen = false;
        }
    }

    for (TActorIterator<ADoorObject> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        if (ActorItr->ActorHasTag("Door3"))
        {
            ADoorObject* pTempEvent = Cast<ADoorObject>(*ActorItr);
            pDoor3 = pTempEvent;
            pDoor3->SetDoorToBroken();
            bDoor3isOpen = false;
        }
    }

    iNumOfActiveEvents = 0;
    GetWorldTimerManager().SetTimer(InitTimer, this, &AStellarFeverTutorialGameModeBase::Init, 2.0f);
    GetWorldTimerManager().SetTimer(CheckActiveEventTimer, this, &AStellarFeverTutorialGameModeBase::CheckActiveEvents, 5.0f, true);

    //Player setup
    ABasePlayer* player = Cast<ABasePlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (player)
    {
        //Set the health component for later
        pPlayerHealthComp = player->GetHealthComponent();
    }


}

void AStellarFeverTutorialGameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    //Clamp the health so the player can't die
    if (pPlayerHealthComp->Health < 20.0f)
    {
        pPlayerHealthComp->Health = 20.0f;
    }
    
}

void AStellarFeverTutorialGameModeBase::CheckActiveEvents()
{
    int itempval = 0;

    for (int i = 0; i < pMainEvents.Num(); i++)
    {
        if (pMainEvents[i]->GetIsActive() == false)
        {
            itempval++;
        }
    }

    if (iNumOfActiveEvents < itempval)
    {
        iNumOfActiveEvents = itempval;
    }

    if (iNumOfActiveEvents >= 2 && bDoor1isOpen == false)
    {
        pDoor1->SetDoorToAutomaticOpen();
        bDoor1isOpen = true;
    }

    if (iNumOfActiveEvents >= 3 && bDoor2isOpen == false)
    {
        pDoor2->SetDoorToAutomaticOpen();
        bDoor2isOpen = true;
    }

    if (iNumOfActiveEvents >= 4 && bDoor3isOpen == false)
    {
        pDoor3->SetDoorToAutomaticOpen();
        bDoor3isOpen = true;
    }

    if (iNumOfActiveEvents >= 5)
    {
        iNumOfActiveEvents++;
        GetWorldTimerManager().SetTimer(CheckActiveEventTimer, this, &AStellarFeverTutorialGameModeBase::CheckActiveEvents, 4.0f, true);
    }

    //I am using 8 so the player at least can see that they are done training
    if (iNumOfActiveEvents >= 8)
    {
        //Set the tutorial complete to true;
        UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
        if (sfInstance)
        {
        	sfInstance->bTutorialComplete = true;
            sfInstance->SaveGame();
        }



        UGameplayStatics::OpenLevel(this, "MainMenuMap");
    }

}

void AStellarFeverTutorialGameModeBase::Init()
{
    for (int i = 0; i < pMainEvents.Num(); i++)
    {
        pMainEvents[i]->Activate();
    }
}
