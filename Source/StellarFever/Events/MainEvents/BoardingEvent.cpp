// Copyright Quarantine Games 2020


#include "BoardingEvent.h"
#include "../SpawnPoint.h"
#include "../EventObject.h"
#include "TeleporterEvent.h"
#include "Net/UnrealNetwork.h"

ABoardingEvent::ABoardingEvent()
{
    //Six Event objects should be present so six indicators are used.
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator1"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator2"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator3"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator4"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator5"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator6"));

    //Attachs them to the object
    for (int i = 0; i < pStatusIndicators.Num(); i++)
    {
        pStatusIndicators[i]->SetupAttachment(RootComponent);
        pStatusIndicators[i]->SetIsReplicated(true);
    }

    pBoardingAudio = CreateDefaultSubobject<UAudioComponent>("BoardingAudioManager");
    pBoardingAudio->SetIsReplicated(true);
    pBoardingAudio->SetupAttachment(RootComponent);

    bCanSpawnBoss = false;

    Tags.Add("MainEvent");
    Tags.Add("Boarding");

    bAlwaysRelevant = true;
    SetReplicates(true);
}

void ABoardingEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //Sets up the eventobjects and ensures they are 
        for (int i = 0; i < pEventObjects.Num(); i++)
        {
            pEventObjects[i]->SetEventOwner(this);
        }

    }

    pBoardingAudio->SetSound(pBoardingMusic);
}

void ABoardingEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABoardingEvent::Activate()
{
    Super::Activate();

    //Changing the text to show if online or offline
    Multicast_ChangeText(true);
}

void ABoardingEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().boarding = true;

    //Resetting all the variables
    iActivatedObjects = 0;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Starts Timers for both the boss enemies and the AI
    SpawnAI();
    ActivateTeleporter();
    GetWorld()->GetTimerManager().SetTimer(SpawnAITimer, this, &ABoardingEvent::SpawnAI, fSpawnAISpeed, true);
}

void ABoardingEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        //Changing the text to show if online or offline
        Multicast_ChangeText(false);
        Multicast_StopMusic();
    }
}

void ABoardingEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
        pEventsManager->GetEventsState().boarding = false;

    //Deactivate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Resets the boss spawning time
    bCanSpawnBoss = false;

    //Turns off the timers when finished
    GetWorld()->GetTimerManager().ClearTimer(SpawnAITimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &ABoardingEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void ABoardingEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void ABoardingEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        Multicast_ChangeStatus(iActivatedObjects);
        iActivatedObjects++;
        if (iActivatedObjects == 6)
        {
            bCanDeactivate = true;
        }
    }
}

void ABoardingEvent::Multicast_ChangeText_Implementation(bool isactive)
{
    if (isactive == true)
    {
        //Setting the text to show red is offline
        pAdditionalText->SetText(FText::FromString("Offline"));
    }

    if (isactive == false)
    {
        //Setting the text to show it is working and online
        pAdditionalText->SetText(FText::FromString("Online"));
    }
}

void ABoardingEvent::Multicast_PlayMusic_Implementation()
{
    if (pBoardingMusic != nullptr)
    {
        pBoardingAudio->FadeIn(25.0f);
    }
}

void ABoardingEvent::Multicast_StopMusic_Implementation()
{
    if (pBoardingMusic != nullptr)
    {
        pBoardingAudio->FadeOut(4.0f, 0.0f);
    }
}

void ABoardingEvent::SpawnAI()
{
    Server_SpawnAI();
}

void ABoardingEvent::Server_SpawnAI_Implementation()
{
    //Will spawn a random value of 1-ucNumOf for each enemy type except the scavengers which is hard set.
    if (ucNumOfRanged > 0)
    {
        uint8 randshields = FMath::RandRange(0, ucNumOfShields);
        uint8 randranged = FMath::RandRange(1, ucNumOfRanged);
        uint8 randexploders = FMath::RandRange(1, ucNumOfExploders);
        uint8 randmelee = FMath::RandRange(1, ucNumOfMelee);
        uint8 randshotgun = FMath::RandRange(0, ucNumOfShotgun);


        if (pSpawnPoints[0] != nullptr)
        {
            //Spawns all listed enemies in the bps, values can be changed overtime if needed
            pEventsManager->GetEnemyPool()->SpawnMultipleEnemies(ucNumOfScavengers, randshields, randranged, randexploders, randmelee, randshotgun, pSpawnPoints);

            //Spawns the boss on every second spawnAI call
            if (bCanSpawnBoss == true)
            {
                SpawnBoss();
                bCanSpawnBoss = false;
            }
            else
            {
                bCanSpawnBoss = true;
            }
        }
    }
}

void ABoardingEvent::SpawnBoss()
{
    Server_SpawnBoss();
}


void ABoardingEvent::Server_SpawnBoss_Implementation()
{
    //Gets a random boss
    uint8 randval = FMath::RandRange(0, 1);
    //Spawns one boss AI at the first spawnpoint in the array
    pEventsManager->GetEnemyPool()->SpawnBoss(pBossSpawnPoints[0], randval);

    //Spawns two bodyguards with the boss
    if (randval == 0)
    {
        //Spawns shield AI for the ranged boss
        pEventsManager->GetEnemyPool()->SpawnOneEnemy(2, pBossSpawnPoints[1]);
        pEventsManager->GetEnemyPool()->SpawnOneEnemy(2, pBossSpawnPoints[2]);
    }
    if (randval == 1)
    {
        //Spawns shotgun AI for the melee boss
        pEventsManager->GetEnemyPool()->SpawnOneEnemy(6, pBossSpawnPoints[1]);
        pEventsManager->GetEnemyPool()->SpawnOneEnemy(6, pBossSpawnPoints[2]);
    }
}

void ABoardingEvent::ActivateTeleporter()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pTeleporterEvent != nullptr)
        {
            pTeleporterEvent->Activate();
        }
    }
}

void ABoardingEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABoardingEvent, iActivatedObjects);
    DOREPLIFETIME(ABoardingEvent, pEventObjects);
}
