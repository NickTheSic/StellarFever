// Copyright Quarantine Games 2020


#include "TeleporterEvent.h"
#include "../ShipSystems/Teleporter.h"
#include "../EnemyShip.h"
#include "Net/UnrealNetwork.h"

ATeleporterEvent::ATeleporterEvent()
{
    Tags.Add("Teleport");
}

void ATeleporterEvent::BeginPlay()
{
    Super::BeginPlay();
}

void ATeleporterEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATeleporterEvent::Activate()
{
    Super::Activate();
}

void ATeleporterEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().teleport = true;

    //Activates the Teleporter to be used
    pTeleporter->Activate();
    pEnemyShip->Activate();

    //if players really want to they can just turn off the event
    bCanDeactivate = true;
}

void ATeleporterEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();
    }
}

void ATeleporterEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().teleport = false;

    //Stops the teleporter from being used
    pTeleporter->Deactivate();
    pEnemyShip->Deactivate();
}

void ATeleporterEvent::AddPlayer(ABasePlayer* player)
{
    pPlayersOnShip.Add(player);
}

void ATeleporterEvent::RemovePlayer(ABasePlayer* player)
{
    //Does a quick check if the player exists in the array and removes it from the array
    for (int i = 0; i < pPlayersOnShip.Num(); i++)
    {
        if (pPlayersOnShip[i] == player)
            pPlayersOnShip.RemoveAt(i);
    }
}
