// Copyright Quarantine Games 2020


#include "BridgeEvent.h"
#include "../EventObject.h"
#include "../EventsManager.h"
#include "EngineEvent.h"
#include "Net/UnrealNetwork.h"

ABridgeEvent::ABridgeEvent()
{
    //Five Event objects should be present so five indicators are used.
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

    pBridgeAudio = CreateDefaultSubobject<UAudioComponent>("BridgeAudioManager");
    pBridgeAudio->SetIsReplicated(true);
    pBridgeAudio->SetupAttachment(RootComponent);

    Tags.Add("MainEvent");
    Tags.Add("Bridge");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void ABridgeEvent::BeginPlay()
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

    pBridgeAudio->SetSound(pBridgeMusic);
}

void ABridgeEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABridgeEvent::Activate()
{
    Super::Activate();
}

void ABridgeEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().bridge = true;

    //Resetting all the variables
    iActivatedObjects = 0;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Changing the text to show if online or offline
    Multicast_ChangeText(true);

    //Start both Timers to call respective functions using blueprinted values
    GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &ABridgeEvent::TriggerEvent, fMinorEventSpeed);
    GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &ABridgeEvent::TriggerMainEvent, fMainEventSpeed);
    GetWorld()->GetTimerManager().SetTimer(ScreenShakeTimer, this, &ABridgeEvent::TriggerScreenShake, fScreenShakeSpeed);

    //Stops the game time elongating the game
    if (pEventsManager)
    {
        pEventsManager->StopGlobalTimer();
    }
}

void ABridgeEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void ABridgeEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().bridge = false;

    //Deactivate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Turn off all Timers
    GetWorld()->GetTimerManager().ClearTimer(MinorEventTimer);
    GetWorld()->GetTimerManager().ClearTimer(ScreenShakeTimer);
    GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);

    //Changing the text to show if online or offline
    Multicast_ChangeText(false);

    //Restarts the game timer allowing time to decrement again
    if (pEventsManager)
    {
        pEventsManager->StartGlobalTimer();
    }

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &ABridgeEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void ABridgeEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void ABridgeEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        float nextspeed = GetWorld()->GetTimerManager().GetTimerRemaining(MainEventTimer);
        nextspeed += fBonusSpeed;
        GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);
        GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &ABridgeEvent::TriggerMainEvent, nextspeed);
        Multicast_ChangeStatus(iActivatedObjects);
        iActivatedObjects++;
        if (iActivatedObjects == 5)
        {
            bCanDeactivate = true;
        }
    }
}

void ABridgeEvent::TriggerEvent()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pEventsManager != nullptr)
        {
            //Activates a minor event from the event manager
            pEventsManager->TriggerMinorEvent();
        }
    }
}

void ABridgeEvent::TriggerMainEvent()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pEventsManager != nullptr)
        {
            //Activates a minor event from the event manager
            pEventsManager->TriggerMainEvent();
            ResetEvent();
        }
    }
}

void ABridgeEvent::TriggerScreenShake()
{
    //TODO need to set up screen shaking
}

void ABridgeEvent::Multicast_ChangeText_Implementation(bool isactive)
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

void ABridgeEvent::Multicast_PlayMusic_Implementation()
{
    if (pBridgeMusic != nullptr)
    {
        pBridgeAudio->FadeIn(25.0f);
    }
}

void ABridgeEvent::Multicast_StopMusic_Implementation()
{
    if (pBridgeMusic != nullptr)
    {
        pBridgeAudio->FadeOut(4.0f, 0.0f);
    }
}

void ABridgeEvent::ResetEvent()
{
    Server_ResetEvent();
    Multicast_ResetEvent();
}

void ABridgeEvent::Multicast_ResetEvent_Implementation()
{
    //Sets them up to return to a red state to properly display how many are active
    for (int i = 0; i < pStatusIndicators.Num(); i++)
    {
        pStatusIndicators[i]->SetMaterial(0, pRedMaterial);
    }
}

void ABridgeEvent::Server_ResetEvent_Implementation()
{
    //Deactivate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Resets the activated events value
    iActivatedObjects = 0;
    bCanDeactivate = false;

    //Removes the timer so it can be reset upon first activating an eventobject
    GetWorld()->GetTimerManager().ClearTimer(MainEventTimer);
    GetWorld()->GetTimerManager().SetTimer(MainEventTimer, this, &ABridgeEvent::TriggerMainEvent, fMainEventSpeed);
}

void ABridgeEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABridgeEvent, iActivatedObjects);
    DOREPLIFETIME(ABridgeEvent, pEventObjects);
}
