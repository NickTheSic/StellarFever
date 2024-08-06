// Copyright Quarantine Games 2020


#include "EnemyShipEvent.h"
#include "../PickupItems/ShellObject.h"
#include "../SpawnPoint.h"
#include "BoardingEvent.h"
#include "TeleporterEvent.h"
#include "Net/UnrealNetwork.h"

AEnemyShipEvent::AEnemyShipEvent()
{
    //Setting up the main colliders
    pObjectColliderLeft = CreateDefaultSubobject<UBoxComponent>("Object Collider Left");
    pObjectColliderLeft->SetCollisionProfileName("OverlapAllDynamic");
    pObjectColliderLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pObjectColliderLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pObjectColliderLeft->SetupAttachment(RootComponent);

    pObjectColliderRight = CreateDefaultSubobject<UBoxComponent>("Object Collider Right");
    pObjectColliderRight->SetCollisionProfileName("OverlapAllDynamic");
    pObjectColliderRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pObjectColliderRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pObjectColliderRight->SetupAttachment(RootComponent);

    //Mesh where the object must be brought to
    pDropOffMeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>("DropOffMeshLeft");
    pDropOffMeshLeft->SetCollisionProfileName("NoCollision");
    pDropOffMeshLeft->SetIsReplicated(true);
    pDropOffMeshLeft->SetupAttachment(RootComponent);

    pDropOffMeshRight = CreateDefaultSubobject<USkeletalMeshComponent>("DropOffMeshRight");
    pDropOffMeshRight->SetCollisionProfileName("NoCollision");
    pDropOffMeshRight->SetIsReplicated(true);
    pDropOffMeshRight->SetupAttachment(RootComponent);

    //Collision for the meshs
    pCollisionLeft = CreateDefaultSubobject<UStaticMeshComponent>("LeftCollision");
    pCollisionLeft->SetCollisionProfileName("BlockAll");
    pCollisionLeft->SetVisibility(false);
    pCollisionLeft->SetupAttachment(RootComponent);

    pCollisionRight = CreateDefaultSubobject<UStaticMeshComponent>("RightCollision");
    pCollisionRight->SetCollisionProfileName("BlockAll");
    pCollisionRight->SetVisibility(false);
    pCollisionRight->SetupAttachment(RootComponent);

    pEnemyShipAudio = CreateDefaultSubobject<UAudioComponent>("EnemyShipAudioManager");
    pEnemyShipAudio->SetIsReplicated(true);
    pEnemyShipAudio->SetupAttachment(RootComponent);

    //Setting variables up
    iBroughtItems = 0;

    //Tag to ensure it is a main event interactable
    Tags.Add("MainEvent");
    Tags.Add("EnemyShip");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void AEnemyShipEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //The overlap event when the object is in the right place
        pObjectColliderLeft->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyShipEvent::OnColliderOverlapLeft);
        pObjectColliderLeft->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShipEvent::OnColliderOverlapLeft);

        pObjectColliderRight->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyShipEvent::OnColliderOverlapRight);
        pObjectColliderRight->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShipEvent::OnColliderOverlapRight);

        //Creates the objects to a pool that can be spawned properly with deactivated and activated
        GenerateObjects();

    }

    pEnemyShipAudio->SetSound(pEnemyShipMusic);
}

void AEnemyShipEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyShipEvent::Activate()
{
    Super::Activate();
    Multicast_ChangeText();
}

void AEnemyShipEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();

    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().enemyship = true;

    //Sets up the timer to start activating minor events.
    GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &AEnemyShipEvent::TriggerEvent, fMinorEventSpeed, true);

    //Sets up the timer to start the boarding event.
    GetWorld()->GetTimerManager().SetTimer(BoardingTimer, this, &AEnemyShipEvent::TriggerBoarding, fBoardingEventSpeed);

    //Ensures all brought objects are set to 0
    iBroughtItems = 0;

    //Enables the text to show the enemy ship health
    pAdditionalText->SetVisibility(true);

    //Spawns the oxygen objects that will auto delete once brought back to the event.
    for (int i = 0; i < pSpawnPoints.Num(); i++)
    {
        if (pShellObjectItems[i]->bIsActive == false)
        {
            pShellObjectItems[i]->SetActorLocation(pSpawnPoints[i]->GetActorLocation());
            pShellObjectItems[i]->Activate();
        }
    }

}

void AEnemyShipEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void AEnemyShipEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().enemyship = false;

    //Safety to clear all timers just in case
    GetWorld()->GetTimerManager().ClearTimer(MinorEventTimer);
    GetWorld()->GetTimerManager().ClearTimer(BoardingTimer);

    //Hides the progress of the event until active again
    pAdditionalText->SetVisibility(false);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &AEnemyShipEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void AEnemyShipEvent::Server_CreateVisuals_Implementation()
{
    //used to change the text
    iBroughtItems++;
    Multicast_ChangeText();
}

void AEnemyShipEvent::Multicast_ChangeText_Implementation()
{
    if (iBroughtItems == 0)
    {
        pAdditionalText->SetText(FText::FromString("Enemy Hull Integrity: 100%"));
    }
    else if (iBroughtItems == 1)
    {
        pAdditionalText->SetText(FText::FromString("Enemy Hull Integrity: 67%"));
    }
    else if (iBroughtItems == 2)
    {
        pAdditionalText->SetText(FText::FromString("Enemy Hull Integrity: 33%"));
    }
}

void AEnemyShipEvent::Multicast_Animation_Implementation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop)
{
    //Plays the animation given and sees if it loops or not
    mesh->PlayAnimation(anim, loop);
}

void AEnemyShipEvent::TriggerEvent()
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

void AEnemyShipEvent::TriggerBoarding()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pBoardingEvent->bIsActive == false)
        {
            //Will activate the boarding event
            pBoardingEvent->Activate();
        }
        else
        {
            pBoardingEvent->SpawnAI();
        }
    }
}

void AEnemyShipEvent::GenerateObjects()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Spawns the shell objects that will auto delete once brought back to the event.
        for (int i = 0; i < pSpawnPoints.Num(); i++)
        {
            FActorSpawnParameters SpawnParams;
            AShellObject* pTempActor = GetWorld()->SpawnActor<AShellObject>(pShellObject, pSpawnPoints[i]->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
            pTempActor->Deactivate();
            pShellObjectItems.Add(pTempActor);
        }
    }
}

void AEnemyShipEvent::Multicast_PlayMusic_Implementation()
{
    if (pEnemyShipMusic != nullptr)
    {
        pEnemyShipAudio->FadeIn(25.0f);
    }
}

void AEnemyShipEvent::Multicast_StopMusic_Implementation()
{
    if (pEnemyShipMusic != nullptr)
    {
        pEnemyShipAudio->FadeOut(4.0f, 0.0f);
    }
}

void AEnemyShipEvent::OnColliderOverlapLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //When the object interacts with the collider, it will activate the correct light to change from red to green
    if (iBroughtItems < 3 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("ShellItem"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            Server_CreateVisuals();
            if (pFireAnim != nullptr)
            {
                Multicast_Animation(pDropOffMeshLeft, pFireAnim, false);
            }
            if (pFireSound != nullptr)
            {
                Multicast_PlaySound(pFireSound);
            }
            Multicast_ChangeText();

            if (iBroughtItems == 3)
            {
                bCanDeactivate = true;
                Deactivate();
            }
        }
    }
}

void AEnemyShipEvent::OnColliderOverlapRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //When the object interacts with the collider, it will activate the correct light to change from red to green
    if (iBroughtItems < 3 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("ShellItem"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            Server_CreateVisuals();
            Multicast_Animation(pDropOffMeshRight, pFireAnim, false);
            Multicast_PlaySound(pFireSound);
            Multicast_ChangeText();

            if (iBroughtItems == 3)
            {
                bCanDeactivate = true;
                Deactivate();
            }
        }
    }
}

void AEnemyShipEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyShipEvent, iBroughtItems);
    DOREPLIFETIME(AEnemyShipEvent, pDropOffMeshLeft);
    DOREPLIFETIME(AEnemyShipEvent, pDropOffMeshRight);
}
