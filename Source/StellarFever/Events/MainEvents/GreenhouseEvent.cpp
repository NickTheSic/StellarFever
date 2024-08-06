// Copyright Quarantine Games 2020


#include "GreenhouseEvent.h"
#include "../EventObject.h"
#include "Net/UnrealNetwork.h"

AGreenhouseEvent::AGreenhouseEvent()
{
    //Four Event objects should be present so four indicators are used.
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator1"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator2"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator3"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator4"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator5"));

    //Attachs them to the object
    for (int i = 0; i < pStatusIndicators.Num(); i++)
    {
        pStatusIndicators[i]->SetupAttachment(RootComponent);
        pStatusIndicators[i]->SetIsReplicated(true);
    }

    //Resetting all the variables
    iActivatedObjects = 0;

    Tags.Add("MainEvent");
    Tags.Add("Greenhouse");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void AGreenhouseEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //Sets the event objects owner to this
        for (int i = 0; i < pEventObjects.Num(); i++)
        {
            pEventObjects[i]->SetEventOwner(this);
        }

    }
}

void AGreenhouseEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGreenhouseEvent::Activate()
{
    Super::Activate();

    //Changing the text to show if online or offline
    Multicast_ChangeText(true);
}

void AGreenhouseEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().greenhouse = true;

    //Resetting all the variables
    iActivatedObjects = 0;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Safety check to find all players before decrementing, may help netowrking
    SetupPlayerPointer();

    //Starts the Health lowering function call timer
    GetWorld()->GetTimerManager().SetTimer(OxygenDecrementTimer, this, &AGreenhouseEvent::LowerOxygen, fOxygenDecrementSpeed, true);
}

void AGreenhouseEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        //Changing the text to show if online or offline
        Multicast_ChangeText(true);
    }
}

void AGreenhouseEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().greenhouse = false;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Removes the health draining timer
    GetWorld()->GetTimerManager().ClearTimer(OxygenDecrementTimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &AGreenhouseEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void AGreenhouseEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void AGreenhouseEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        Multicast_ChangeStatus(iActivatedObjects);
        iActivatedObjects++;
        if (iActivatedObjects == 5)
        {
            bCanDeactivate = true;
        }
    }
}

void AGreenhouseEvent::Multicast_ChangeText_Implementation(bool isactive)
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

void AGreenhouseEvent::LowerOxygen()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Lowers the current oxygen per timer value, and once every few intervals will lower the max oxygen on all players
        for (int i = 0; i < pPlayers.Num(); i++)
        {
            if (pPlayers[i] != nullptr)
            {
                if (pPlayers[i]->GetIsOnEnemyShip() == false)
                {
                    pPlayers[i]->DrainOxygen(fOxygenDecrementValue);
                }
            }
        }
    }
}

void AGreenhouseEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGreenhouseEvent, iActivatedObjects);
    DOREPLIFETIME(AGreenhouseEvent, pEventObjects);
}
