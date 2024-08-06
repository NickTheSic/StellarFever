// Copyright Quarantine Games 2020


#include "BathroomEvent.h"
#include "../PickUpItem.h"
#include "Net/UnrealNetwork.h"

ABathroomEvent::ABathroomEvent()
{
    //Creating a seperate collider for accepting the objects spawned by the event
    pObjectCollider = CreateDefaultSubobject<UBoxComponent>("ObjectCollider");
    pObjectCollider->SetCollisionProfileName("OverlapAllDynamic");
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetupAttachment(RootComponent);

    //Setting values
    iBroughtItems = 0;

    //Tag to ensure it is a main event interactable
    Tags.Add("MainEvent");
    Tags.Add("Bathroom");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void ABathroomEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //The overlap event when the object is in the right place
        pObjectCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ABathroomEvent::OnColliderOverlap);
        pObjectCollider->OnComponentBeginOverlap.AddDynamic(this, &ABathroomEvent::OnColliderOverlap);
    }
}

void ABathroomEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABathroomEvent::Activate()
{
    Super::Activate();
}

void ABathroomEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().bathroom = true;

    //Changes the text to online
    Multicast_ChangeText(true);

    //Sets up the timer only if its activated to start functionality
    GetWorld()->GetTimerManager().SetTimer(OxygenDecrementTimer, this, &ABathroomEvent::LowerOxygen, fOxygenDecrementSpeed, true);

    //Starts the Health lowering function call timer
    GetWorld()->GetTimerManager().SetTimer(HealthDecrementTimer, this, &ABathroomEvent::LowerHealth, fHealthDecrementSpeed, true);

    //Ensuring the base values are reset and have proper values
    iBroughtItems = 0;

    //Safety check to find all players before decrementing, may help netowrking
    SetupPlayerPointer();
}

void ABathroomEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();
    }
}

void ABathroomEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().bathroom = false;

    //Changes the text to online
    Multicast_ChangeText(false);

    //Clears the timer so it stops calling the loweroxygen function
    GetWorld()->GetTimerManager().ClearTimer(OxygenDecrementTimer);
    GetWorld()->GetTimerManager().ClearTimer(HealthDecrementTimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &ABathroomEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void ABathroomEvent::LowerOxygen()
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

void ABathroomEvent::LowerHealth()
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
                    pPlayers[i]->DrainHealth(fHealthDecrementValue);
                }
            }
        }
    }
}

void ABathroomEvent::Multicast_ChangeText_Implementation(bool isactive)
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

void ABathroomEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (iBroughtItems < 1 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("Janitor"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            iBroughtItems++;
            if (iBroughtItems == 1)
            {
                bCanDeactivate = true;
                Deactivate();
            }
        }
    }
}

void ABathroomEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABathroomEvent, iBroughtItems);
}
