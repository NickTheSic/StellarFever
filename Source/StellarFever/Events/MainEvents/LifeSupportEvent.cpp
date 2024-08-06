// Copyright Quarantine Games 2020


#include "LifeSupportEvent.h"
#include "../SpawnPoint.h"
#include "../PickupItems/MedbayObject.h"
#include "../EventObject.h"
#include "Net/UnrealNetwork.h"

ALifeSupportEvent::ALifeSupportEvent()
{
    //Creating a seperate collider for accepting the objects spawned by the event
    pObjectCollider = CreateDefaultSubobject<UBoxComponent>("ObjectCollider");
    pObjectCollider->SetCollisionProfileName("OverlapAllDynamic");
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetupAttachment(RootComponent);

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

    //Mesh where the object must be brought to
    pDropOffMesh = CreateDefaultSubobject<USkeletalMeshComponent>("DropOffMesh");
    pDropOffMesh->SetCollisionProfileName("BlockAll");
    pDropOffMesh->SetupAttachment(RootComponent);

    //Mesh where the object must be brought to
    pFixedDropOffMesh = CreateDefaultSubobject<UStaticMeshComponent>("RepairedDropOffMesh");
    pFixedDropOffMesh->SetCollisionProfileName("BlockAll");
    pFixedDropOffMesh->SetupAttachment(RootComponent);

    pLifeSupportAudio = CreateDefaultSubobject<UAudioComponent>("LifeSupportAudioManager");
    pLifeSupportAudio->SetIsReplicated(true);
    pLifeSupportAudio->SetupAttachment(RootComponent);

    //Resetting all the variables
    iBroughtItems = 0;
    iActivatedObjects = 0;

    Tags.Add("MainEvent");
    Tags.Add("LifeSupport");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void ALifeSupportEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //Sets the event objects owner to this
        for (int i = 0; i < pEventObjects.Num(); i++)
        {
            pEventObjects[i]->SetEventOwner(this);
        }

        pObjectCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ALifeSupportEvent::OnColliderOverlap);
        pObjectCollider->OnComponentBeginOverlap.AddDynamic(this, &ALifeSupportEvent::OnColliderOverlap);

        GenerateObjects();
    }

    pLifeSupportAudio->SetSound(pLifeSupportMusic);
}

void ALifeSupportEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALifeSupportEvent::Activate()
{
    Super::Activate();
}

void ALifeSupportEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().lifesupport = true;

    //Resetting all the variables
    iBroughtItems = 0;
    iActivatedObjects = 0;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Safety check to find all players before decrementing, may help netowrking
    SetupPlayerPointer();

    //Spawning the item
    pMedbayObjectItem->SetActorLocation(pSpawnPoints[0]->GetActorLocation());
    pMedbayObjectItem->Activate();

    //Starts the Health lowering function call timer
    GetWorld()->GetTimerManager().SetTimer(HealthDecrementTimer, this, &ALifeSupportEvent::LowerHealth, fHealthDecrementSpeed, true);

    //Will hide the fixed mesh and appear the broken mesh
    pFixedDropOffMesh->SetVisibility(false);
    pDropOffMesh->SetVisibility(true);
}

void ALifeSupportEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void ALifeSupportEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().lifesupport = false;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Removes the health draining timer
    GetWorld()->GetTimerManager().ClearTimer(HealthDecrementTimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &ALifeSupportEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);

    //Will hide the broken mesh and appear the fixed mesh
    pFixedDropOffMesh->SetVisibility(true);
    pDropOffMesh->SetVisibility(false);
}

void ALifeSupportEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void ALifeSupportEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        Multicast_ChangeStatus(iBroughtItems + iActivatedObjects);
        iActivatedObjects++;
        if (iActivatedObjects == 2 && iBroughtItems == 1)
        {
            bCanDeactivate = true;
        }
    }
}

void ALifeSupportEvent::GenerateObjects()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Spawns the medbay objects that will auto delete once brought back to the event.
        for (int i = 0; i < pSpawnPoints.Num(); i++)
        {
            FActorSpawnParameters SpawnParams;
            AMedbayObject* pTempActor = GetWorld()->SpawnActor<AMedbayObject>(pMedbayObject, pSpawnPoints[i]->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
            pTempActor->Deactivate();
            pMedbayObjectItem = pTempActor;
        }
    }
}

void ALifeSupportEvent::LowerHealth()
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

void ALifeSupportEvent::Multicast_PlayMusic_Implementation()
{
    if (pLifeSupportMusic != nullptr)
    {
        pLifeSupportAudio->FadeIn(25.0f);
    }
}

void ALifeSupportEvent::Multicast_StopMusic_Implementation()
{
    if (pLifeSupportMusic != nullptr)
    {
        pLifeSupportAudio->FadeOut(4.0f, 0.0f);
    }
}

void ALifeSupportEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (iBroughtItems < 1 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("MedbayObject"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            Multicast_ChangeStatus(iBroughtItems + iActivatedObjects);
            iBroughtItems++;

            //Will hide the broken mesh and appear the fixed mesh
            pFixedDropOffMesh->SetVisibility(true);
            pDropOffMesh->SetVisibility(false);

            if (iActivatedObjects == 2 && iBroughtItems == 1)
            {
                bCanDeactivate = true;
            }
        }
    }
}

void ALifeSupportEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALifeSupportEvent, iBroughtItems);
    DOREPLIFETIME(ALifeSupportEvent, iActivatedObjects);
    DOREPLIFETIME(ALifeSupportEvent, pEventObjects);
}