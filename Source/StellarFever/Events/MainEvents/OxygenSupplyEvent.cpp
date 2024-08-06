// Fill out your copyright notice in the Description page of Project Settings.


#include "OxygenSupplyEvent.h"
#include "../PickupItems/OxygenSupplyActor.h"
#include "../SpawnPoint.h"
#include "../../Components/StatusComponent.h"
#include "Net/UnrealNetwork.h"

AOxygenSupplyEvent::AOxygenSupplyEvent()
{
    //Creating a seperate collider for accepting the objects spawned by the event
    pObjectCollider = CreateDefaultSubobject<UBoxComponent>("ObjectCollider");
    pObjectCollider->SetCollisionProfileName("OverlapAllDynamic");
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    pObjectCollider->SetupAttachment(RootComponent);

    //Setting up the appropriate amount indicators that will appear on the event
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

    pDropOffMesh->SetCollisionProfileName("NoCollision");
    pDropOffMesh->SetIsReplicated(true);
    pDropOffMesh->SetupAttachment(RootComponent);

    //the arms that will animate
    pArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ArmsMesh");
    pArmsMesh->SetCollisionProfileName("NoCollision");
    pArmsMesh->SetIsReplicated(true);
    pArmsMesh->SetupAttachment(RootComponent);

    pOxygenAudio = CreateDefaultSubobject<UAudioComponent>("OxygenAudioManager");
    pOxygenAudio->SetIsReplicated(true);
    pOxygenAudio->SetupAttachment(RootComponent);

    //Setting values
    iBroughtItems = 0;

    //Tag to ensure it is a main event interactable
    Tags.Add("MainEvent");
    Tags.Add("OxygenSupply");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void AOxygenSupplyEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //The overlap event when the object is in the right place
        pObjectCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AOxygenSupplyEvent::OnColliderOverlap);
        pObjectCollider->OnComponentBeginOverlap.AddDynamic(this, &AOxygenSupplyEvent::OnColliderOverlap);

        //Creates the objects to be spawned
        GenerateObjects();
    }

    pOxygenAudio->SetSound(pOxygenMusic);
}

void AOxygenSupplyEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AOxygenSupplyEvent::Activate()
{
    Super::Activate(); 
}

void AOxygenSupplyEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().oxygen = true;

    //Sets up the timer only if its activated to start functionality
    GetWorld()->GetTimerManager().SetTimer(OxygenDecrementTimer, this, &AOxygenSupplyEvent::LowerOxygen, fOxygenDecrementSpeed, true);

    //Ensuring the base values are reset and have proper values
    iBroughtItems = 0;

    //Safety check to find all players before decrementing, may help netowrking
    SetupPlayerPointer();

    //The looping anim for the little orb to stop spinning
    if (pLoopingAnim != nullptr)
    {
        pDropOffMesh->Stop();
    }

    //Spawns the oxygen objects that will auto delete once brought back to the event.
    for (int i = 0; i < pSpawnPoints.Num(); i++)
    {
        pOxygenObjectItems[i]->SetActorLocation(pSpawnPoints[i]->GetActorLocation());
        pOxygenObjectItems[i]->Activate();
    }

}

void AOxygenSupplyEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void AOxygenSupplyEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
    if (pEventsManager != nullptr)
    pEventsManager->GetEventsState().oxygen = false;

    //Clears the timer so it stops calling the loweroxygen function
    GetWorld()->GetTimerManager().ClearTimer(OxygenDecrementTimer);

    //The looping anim for the little orb to spin
    if (pLoopingAnim != nullptr)
    {
        Multicast_Animation(pDropOffMesh, pLoopingAnim, true);
    }

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &AOxygenSupplyEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);
}

void AOxygenSupplyEvent::LowerOxygen()
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

void AOxygenSupplyEvent::GenerateObjects()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Spawns the oxygen objects that will auto delete once brought back to the event.
        for (int i = 0; i < pSpawnPoints.Num(); i++)
        {
            FActorSpawnParameters SpawnParams;
            AOxygenSupplyActor* pTempActor = GetWorld()->SpawnActor<AOxygenSupplyActor>(pOxygenObject, pSpawnPoints[i]->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
            pTempActor->Deactivate();
            pOxygenObjectItems.Add(pTempActor);
        }
    }
}

void AOxygenSupplyEvent::Multicast_PlayMusic_Implementation()
{
    if (pOxygenMusic != nullptr)
    {
        pOxygenAudio->FadeIn(25.0f);
    }
}

void AOxygenSupplyEvent::Multicast_StopMusic_Implementation()
{
    if (pOxygenMusic != nullptr)
    {
        pOxygenAudio->FadeOut(4.0f, 0.0f);
    }
}

void AOxygenSupplyEvent::Multicast_Animation_Implementation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop)
{
    //Plays the animation given and sees if it loops or not
    mesh->PlayAnimation(anim, loop);
}

void AOxygenSupplyEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //Deletes the object and increases a counter to increment how many more are needed.
    if (iBroughtItems < 3 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("OxygenItem"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            Multicast_ChangeStatus(iBroughtItems);

            //A quick check of which anim to play
            if (iBroughtItems == 0)
            {
                if (pFirstArmAnim != nullptr)
                {
                    Multicast_Animation(pArmsMesh, pFirstArmAnim, false);
                }
            }

            if (iBroughtItems == 1)
            {
                if (pSecondArmAnim != nullptr)
                {
                    Multicast_Animation(pArmsMesh, pSecondArmAnim, false);
                }
            }

            if (iBroughtItems == 2)
            {
                if (pThirdArmAnim != nullptr)
                {
                    Multicast_Animation(pArmsMesh, pThirdArmAnim, false);
                }
            }

            iBroughtItems++;
            if (iBroughtItems == 3)
            {
                bCanDeactivate = true;
            }
        }
    }
}

void AOxygenSupplyEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOxygenSupplyEvent, iBroughtItems);
    DOREPLIFETIME(AOxygenSupplyEvent, pDropOffMesh);
    DOREPLIFETIME(AOxygenSupplyEvent, pArmsMesh);
}