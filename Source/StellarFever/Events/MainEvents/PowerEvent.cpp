// Copyright Quarantine Games 2020


#include "PowerEvent.h"
#include "../PickupItems/BatteryObject.h"
#include "../SpawnPoint.h"
#include "../EventObject.h"
#include "../ShipSystems/DoorObject.h"
#include "../ShipSystems/EmergencyLight.h"
#include "Net/UnrealNetwork.h"

APowerEvent::APowerEvent()
{
    //Creating a seperate collider for accepting the objects spawned by the event
    pObjectCollider = CreateDefaultSubobject<UBoxComponent>("ObjectCollider");
    pObjectCollider->SetCollisionProfileName("OverlapAllDynamic");
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetupAttachment(RootComponent);

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

    //Setting up the Meshes so they exist in game
    pBatteryMesh.Add(CreateDefaultSubobject<USkeletalMeshComponent>("FirstBattery"));
    pBatteryMesh[0]->SetCollisionProfileName("BlockAll");
    pBatteryMesh[0]->SetupAttachment(RootComponent);
    pBatteryMesh.Add(CreateDefaultSubobject<USkeletalMeshComponent>("SecondBattery"));
    pBatteryMesh[1]->SetCollisionProfileName("BlockAll");
    pBatteryMesh[1]->SetupAttachment(RootComponent);

    //Meshes where the object must be brought to
    pDropOffMeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>("DropOffMeshLeft");
    pDropOffMeshLeft->SetCollisionProfileName("BlockAll");
    pDropOffMeshLeft->SetupAttachment(RootComponent);
    pDropOffMeshRight = CreateDefaultSubobject<USkeletalMeshComponent>("DropOffMeshRight");
    pDropOffMeshRight->SetCollisionProfileName("BlockAll");
    pDropOffMeshRight->SetupAttachment(RootComponent);

    pPowerAudio = CreateDefaultSubobject<UAudioComponent>("PowerAudioManager");
    pPowerAudio->SetIsReplicated(true);
    pPowerAudio->SetupAttachment(RootComponent);

    //Resetting all the variables
    iBroughtItems = 0;
    iActivatedObjects = 0;

    Tags.Add("MainEvent");
    Tags.Add("Power");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void APowerEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {

        //Sets the event objects owner to this
        for (int i = 0; i < pEventObjects.Num(); i++)
        {
            pEventObjects[i]->SetEventOwner(this);
        }

        pObjectCollider->OnComponentBeginOverlap.RemoveDynamic(this, &APowerEvent::OnColliderOverlap);
        pObjectCollider->OnComponentBeginOverlap.AddDynamic(this, &APowerEvent::OnColliderOverlap);

        //Spawns the Items to be set up
        GenerateObjects();
    }

    pPowerAudio->SetSound(pPowerMusic);
}

void APowerEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (iBroughtItems == 1 && bIsActive == true)
    {
        pBatteryMesh[0]->SetVisibility(true);
    }
    if (iBroughtItems == 2 && bIsActive == true)
    {
        pBatteryMesh[1]->SetVisibility(true);
    }

}

void APowerEvent::Activate()
{
    Super::Activate();
}

void APowerEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
        pEventsManager->GetEventsState().power = true;

    //Resetting all the variables
    iBroughtItems = 0;
    iActivatedObjects = 0;

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Spawns the two batteryobjects at the two spawnpoints listed
    for (int i = 0; i < pSpawnPoints.Num(); i++)
    {
        if (pBatteryObjectItems[i]->bIsActive == false)
        {
            pBatteryObjectItems[i]->SetActorLocation(pSpawnPoints[i]->GetActorLocation());
            pBatteryObjectItems[i]->Activate();
            pBatteryMesh[i]->SetVisibility(false);
            pBatteryMesh[i]->Stop();
        }
    }

    //Stops the anims on the drop off meshs
    pDropOffMeshLeft->Stop();
    pDropOffMeshRight->Stop();
}

void APowerEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void APowerEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
        pEventsManager->GetEventsState().power = false;

    //Makes sure meshs are visible in the mesh
    for (int i = 0; i < pBatteryMesh.Num(); i++)
    {
        pBatteryMesh[i]->SetVisibility(true);
    }

    //Plays the looping anim for both drop off points
    if (pMeshAnim != nullptr)
    {
        Multicast_Animation(pDropOffMeshLeft, pMeshAnim, true);
        Multicast_Animation(pDropOffMeshRight, pMeshAnim, true);
    }
    PlayFirstBatteryLoop();
    PlaySecondBatteryLoop();

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &APowerEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void APowerEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void APowerEvent::Server_EventObjectComplete_Implementation()
{
    Super::Server_EventObjectComplete_Implementation();

    //Checking and ensuring all event objects are deactivated, and setting the status meshs accordingly
    if (iActivatedObjects < pEventObjects.Num() && bIsActive == true)
    {
        Multicast_ChangeStatus(iBroughtItems + iActivatedObjects);
        iActivatedObjects++;
        if (iActivatedObjects == 1 && iBroughtItems == 2)
        {
            bCanDeactivate = true;
        }
    }
}

void APowerEvent::GenerateObjects()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Spawns the engine objects that will auto delete once brought back to the event.
        for (int i = 0; i < pSpawnPoints.Num(); i++)
        {
            FActorSpawnParameters SpawnParams;
            ABatteryObject* pTempActor = GetWorld()->SpawnActor<ABatteryObject>(pBatteryObject, pSpawnPoints[i]->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
            pTempActor->Deactivate();
            pBatteryObjectItems.Add(pTempActor);
        }
    }
}

void APowerEvent::PlayFirstBatteryLoop()
{
    if (pLoopingAnim != nullptr)
    {
        Multicast_Animation(pBatteryMesh[0], pLoopingAnim, true);
    }
}

void APowerEvent::PlaySecondBatteryLoop()
{
    if (pLoopingAnim != nullptr)
    {
        Multicast_Animation(pBatteryMesh[1], pLoopingAnim, true);
    }
}

void APowerEvent::Multicast_Animation_Implementation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop)
{
    //Plays the animation given and sees if it loops or not
    mesh->PlayAnimation(anim, loop);
}

void APowerEvent::Multicast_PlayMusic_Implementation()
{
    if (pPowerMusic != nullptr)
    {
        pPowerAudio->FadeIn(25.0f);
    }
}

void APowerEvent::Multicast_StopMusic_Implementation()
{
    if (pPowerMusic != nullptr)
    {
        pPowerAudio->FadeOut(4.0f, 0.0f);
    }
}

void APowerEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (iBroughtItems < 2 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("BatteryItem"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            Multicast_ChangeStatus(iBroughtItems + iActivatedObjects);
            //A quick check of which anim to play
            if (iBroughtItems == 0)
            {
                if (pBatteryMesh[0] != nullptr)
                {
                    Multicast_Animation(pBatteryMesh[0], pBatteryAnim, false);
                    GetWorld()->GetTimerManager().SetTimer(FirstBatteryTimer, this, &APowerEvent::PlayFirstBatteryLoop, 5.0f);
                }
            }

            if (iBroughtItems == 1)
            {
                if (pBatteryMesh[1] != nullptr)
                {
                    Multicast_Animation(pBatteryMesh[1], pBatteryAnim, false);
                    GetWorld()->GetTimerManager().SetTimer(SecondBatteryTimer, this, &APowerEvent::PlaySecondBatteryLoop, 5.0f);
                }
            }
            iBroughtItems++;

            if (iActivatedObjects == 1 && iBroughtItems == 2)
            {
                bCanDeactivate = true;
            }
        }
    }
}

void APowerEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APowerEvent, iBroughtItems);
    DOREPLIFETIME(APowerEvent, iActivatedObjects);
    DOREPLIFETIME(APowerEvent, pBatteryMesh);
    DOREPLIFETIME(APowerEvent, pEventObjects);
}