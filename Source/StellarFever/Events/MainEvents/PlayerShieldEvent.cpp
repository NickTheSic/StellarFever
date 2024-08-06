// Copyright Quarantine Games 2020


#include "PlayerShieldEvent.h"
#include "../EventObject.h"
#include "Net/UnrealNetwork.h"

APlayerShieldEvent::APlayerShieldEvent()
{
    //Four Event objects should be present so four indicators are used.
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator1"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator2"));
    pStatusIndicators.Add(CreateDefaultSubobject<UStaticMeshComponent>("Indicator3"));

    //Attachs them to the object
    for (int i = 0; i < pStatusIndicators.Num(); i++)
    {
        pStatusIndicators[i]->SetupAttachment(RootComponent);
        pStatusIndicators[i]->SetIsReplicated(true);
    }

    pShieldAudio = CreateDefaultSubobject<UAudioComponent>("ShieldAudioManager");
    pShieldAudio->SetIsReplicated(true);
    pShieldAudio->SetupAttachment(RootComponent);

    //Resetting all the variables
    iActivatedObjects = 0;

    Tags.Add("MainEvent");
    Tags.Add("PlayerShield");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void APlayerShieldEvent::BeginPlay()
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

    pShieldAudio->SetSound(pShieldMusic);
}

void APlayerShieldEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APlayerShieldEvent::Activate()
{
    Super::Activate();
}

void APlayerShieldEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().playershield = true;

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
    GetWorld()->GetTimerManager().SetTimer(ShieldDecrementTimer, this, &APlayerShieldEvent::LowerShield, fShieldDecrementSpeed, true);
}

void APlayerShieldEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void APlayerShieldEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().playershield = false;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Removes the health draining timer
    GetWorld()->GetTimerManager().ClearTimer(ShieldDecrementTimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &APlayerShieldEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void APlayerShieldEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void APlayerShieldEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        Multicast_ChangeStatus(iActivatedObjects);
        iActivatedObjects++;
        if (iActivatedObjects == 3)
        {
            bCanDeactivate = true;
        }
    }
}

void APlayerShieldEvent::LowerShield()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Cycles through all active players and lowers their health slowly
        for (int i = 0; i < pPlayers.Num(); i++)
        {
            if (pPlayers[i] != nullptr)
            {
                if (pPlayers[i]->GetIsOnEnemyShip() == false)
                {
                    pPlayers[i]->DrainHitShield(fShieldDecrementValue);
                }
            }
        }
    }
}

void APlayerShieldEvent::Multicast_PlayMusic_Implementation()
{
    if (pShieldMusic != nullptr)
    {
        pShieldAudio->FadeIn(25.0f);
    }
}

void APlayerShieldEvent::Multicast_StopMusic_Implementation()
{
    if (pShieldMusic != nullptr)
    {
        pShieldAudio->FadeOut(4.0f, 0.0f);
    }
}

void APlayerShieldEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerShieldEvent, iActivatedObjects);
    DOREPLIFETIME(APlayerShieldEvent, pEventObjects);
}