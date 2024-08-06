// Copyright Quarantine Games 2020


#include "DepressureEvent.h"
#include "../EventObject.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ADepressureEvent::ADepressureEvent()
{
    //Three Event objects should be present so three indicators are used.
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator1"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator2"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator3"));

    //Attachs them to the object
    for (int i = 0; i < pStatusIndicators.Num(); i++)
    {
        pStatusIndicators[i]->SetupAttachment(RootComponent);
        pStatusIndicators[i]->SetIsReplicated(true);
    }

    Tags.Add("MainEvent");
    Tags.Add("Depressure");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void ADepressureEvent::BeginPlay()
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
}

void ADepressureEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADepressureEvent::Activate()
{
    Super::Activate();
}

void ADepressureEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().depressure = true;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Safety check to find all players before decrementing, may help netowrking
    SetupPlayerPointer();

    //Resets the values for the eventobjects
    iActivatedObjects = 0;

    //Will loop through all players and set their gravity to slightly less intense
    for (int i = 0; i < pPlayers.Num(); i++)
    {
        pPlayers[i]->GetCharacterMovement()->GravityScale = 0.25f;
    }

    //Sets up the timer to break the events
    GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &ADepressureEvent::BreakMainEvent, fAITimerLength);
    GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &ADepressureEvent::TriggerMinorEvent, fMinorEventSpeed);

    //TODO add the weapon sway to all players
}

void ADepressureEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void ADepressureEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().depressure = false;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Will loop through all players and set their gravity to slightly less intense
    for (int i = 0; i < pPlayers.Num(); i++)
    {
        pPlayers[i]->GetCharacterMovement()->GravityScale = 1.0f;
    }

    //Removes the timer that will break a main event
    GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);
    GetWorld()->GetTimerManager().ClearTimer(MinorEventTimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &ADepressureEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void ADepressureEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void ADepressureEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        Multicast_ChangeStatus(iActivatedObjects);
        iActivatedObjects++;
        LowerGravityScale();
        if (iActivatedObjects == 3)
        {
            bCanDeactivate = true;
        }
    }
}

void ADepressureEvent::LowerGravityScale()
{
    //Depending on how many buttons are pressed, will reduce the effects and the timer to activate a main event
    switch (iActivatedObjects) {
    case 1: {

            //Changes the gravity
            for (int i = 0; i < pPlayers.Num(); i++)
            {
                pPlayers[i]->GetCharacterMovement()->GravityScale = 0.4f;
            }
        }
    case 2: {

            //Changes the gravity
            for (int i = 0; i < pPlayers.Num(); i++)
            {
                pPlayers[i]->GetCharacterMovement()->GravityScale = 0.55f;
            }
        }
    case 3: {

            //Changes the gravity
            for (int i = 0; i < pPlayers.Num(); i++)
            {
                pPlayers[i]->GetCharacterMovement()->GravityScale = 0.7f;
            }
        }
    }

    //Changes timer to add more time
    float ftempval = GetWorld()->GetTimerManager().GetTimerRemaining(MainEventTimer);
    ftempval += fBonusEventSpeed;
    GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);
    GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &ADepressureEvent::BreakMainEvent, ftempval);
}

void ADepressureEvent::BreakMainEvent()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pEventsManager != nullptr)
        {
            //Activates a minor event from the event manager
            pEventsManager->TriggerMainEvent();
            GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);
        }
    }
}

void ADepressureEvent::TriggerMinorEvent()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pEventsManager != nullptr)
        {
            //Trigger a random minor event
            pEventsManager->TriggerMinorEvent();
        }
    }
}

void ADepressureEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADepressureEvent, iActivatedObjects);
    DOREPLIFETIME(ADepressureEvent, pEventObjects);
}