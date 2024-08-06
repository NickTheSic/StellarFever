// Copyright Quarantine Games 2020


#include "EngineEvent.h"
#include "../EventObject.h"
#include "../PickupItems/EngineObject.h"
#include "../EventsManager.h"
#include "BridgeEvent.h"
#include "../SpawnPoint.h"
#include "Net/UnrealNetwork.h"
#include "../../Player/BasePlayer.h"

AEngineEvent::AEngineEvent()
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
    pDropOffMesh->SetIsReplicated(true);
    pDropOffMesh->SetupAttachment(RootComponent);

    //The meshs to show that the engine is active
    pInnerMesh = CreateDefaultSubobject<USkeletalMeshComponent>("InnerMesh");
    pInnerMesh->SetIsReplicated(true);
    pCenterMesh = CreateDefaultSubobject<USkeletalMeshComponent>("CenterMesh");
    pCenterMesh->SetIsReplicated(true);
    pOuterMesh = CreateDefaultSubobject<USkeletalMeshComponent>("OuterMesh");
    pOuterMesh->SetIsReplicated(true);
    pInnerMesh->SetupAttachment(RootComponent);
    pCenterMesh->SetupAttachment(RootComponent);
    pOuterMesh->SetupAttachment(RootComponent);

    pEngineAudio = CreateDefaultSubobject<UAudioComponent>("EngineAudioManager");
    pEngineAudio->SetIsReplicated(true);
    pEngineAudio->SetupAttachment(RootComponent);

    //Resetting all the variables
    iBroughtItems = 0;
    iActivatedObjects = 0;

    Tags.Add("MainEvent");
    Tags.Add("Engine");

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void AEngineEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        //Sets the event objects owner to this
        for (int i = 0; i < pEventObjects.Num(); i++)
        {
            pEventObjects[i]->SetEventOwner(this);
        }

        pObjectCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AEngineEvent::OnColliderOverlap);
        pObjectCollider->OnComponentBeginOverlap.AddDynamic(this, &AEngineEvent::OnColliderOverlap);
        pObjectCollider->OnComponentEndOverlap.RemoveDynamic(this, &AEngineEvent::OnColliderExit);
        pObjectCollider->OnComponentEndOverlap.AddDynamic(this, &AEngineEvent::OnColliderExit);

        //Creates all objects in beginplay that will spawn
        GenerateObjects();
    }

    pEngineAudio->SetSound(pEngineMusic);
}

void AEngineEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AEngineEvent::Activate()
{
    //Replace the audio with the both one playing
    if (pOtherEngine != nullptr)
    {
        if (pOtherEngine->GetIsActive() == true)
        {
            pEngineAudio->SetSound(pBothEngineMusic);
        }
    }

    Super::Activate();
}

void AEngineEvent::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
	if (pEventsManager != nullptr)
	{
		if (GetActorLocation().Y < 0)
		{
			pEventsManager->GetEventsState().rengine = true;
		}
		else
		{
			pEventsManager->GetEventsState().lengine = true;
		}
	}
    //Starts the minor event timer
    GetWorld()->GetTimerManager().SetTimer(MinorEventTimer, this, &AEngineEvent::TriggerEvent, fMinorEventSpeed);

    //Resetting all the variables
    iBroughtItems = 0;
    iActivatedObjects = 0;

    //Changing the text to show if online or offline
    Multicast_ChangeText(true);

    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Activate();
    }

    //Stop all anims from playing
    pInnerMesh->Stop();
    pCenterMesh->Stop();
    pOuterMesh->Stop();

    //Spawns the two engineobjects at the two spawnpoints listed
    for (int i = 0; i < pSpawnPoints.Num(); i++)
    {
        if (pEngineObjectItem->bIsActive == false)
        {
            pEngineObjectItem->SetActorLocation(pSpawnPoints[i]->GetActorLocation());
            pEngineObjectItem->Activate();
        }
    }

    //Tells the gamemode that they are active or not
    pEventsManager->SetEngineIsActive(true);
}

void AEngineEvent::Deactivate()
{
    if (bCanDeactivate == true)
    {
        Super::Deactivate();

        Multicast_StopMusic();
    }
}

void AEngineEvent::Server_Deactivate_Implementation()
{
    Super::Server_Deactivate_Implementation();
	if (pEventsManager != nullptr)
	{
		if (GetActorLocation().Y < 0)
		{
			pEventsManager->GetEventsState().rengine = false;
		}
		else
		{
			pEventsManager->GetEventsState().lengine = false;
		}
	}
    //Activate all connected objects
    for (int i = 0; i < pEventObjects.Num(); i++)
    {
        pEventObjects[i]->Deactivate();
    }

    //Changing the text to show if online or offline
    Multicast_ChangeText(false);

    //Plays all the anims to loop through to show the engine is online
    if (pInnerAnim != nullptr && pCenterAnim != nullptr && pOuterAnim != nullptr)
    {
        Multicast_Animation(pInnerMesh, pInnerAnim, true);
        Multicast_Animation(pCenterMesh, pCenterAnim, true);
        Multicast_Animation(pOuterMesh, pOuterAnim, true);
    }

    //Stops the minor event timer
    GetWorld()->GetTimerManager().ClearTimer(MinorEventTimer);

    //Sets up the timer to call the AI to deactivate the event, pauses it immediately so it doesnt decrement right away
    GetWorld()->GetTimerManager().SetTimer(AICollisionTimer, this, &AEngineEvent::Activate, fAITimerLength);
    GetWorld()->GetTimerManager().PauseTimer(AICollisionTimer);

    //Tells the gamemode that they are active or not
    pEventsManager->SetEngineIsActive(false);
}

void AEngineEvent::EventObjectComplete()
{
    Super::EventObjectComplete();
}

void AEngineEvent::Server_EventObjectComplete_Implementation()
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

void AEngineEvent::TriggerEvent()
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

void AEngineEvent::GenerateObjects()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //Spawns the engine objects that will auto delete once brought back to the event.
        for (int i = 0; i < pSpawnPoints.Num(); i++)
        {
            FActorSpawnParameters SpawnParams;
            AEngineObject* pTempActor = GetWorld()->SpawnActor<AEngineObject>(pEngineObject, pSpawnPoints[i]->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
            pTempActor->Deactivate();
            pEngineObjectItem = pTempActor;
        }
    }
}

void AEngineEvent::PlayIdleAnim()
{
    if (pIdleOpenAnim != nullptr)
    {
        Multicast_Animation(pDropOffMesh, pIdleOpenAnim, true);
    }
}

void AEngineEvent::PlayInsertionAnim()
{
    if (pIdleOpenAnim != nullptr)
    {
        Multicast_Animation(pDropOffMesh, pItemAnim, false);
    }
}

void AEngineEvent::Multicast_Animation_Implementation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop)
{
    //Plays the animation given and sees if it loops or not
    mesh->PlayAnimation(anim, loop);
}

void AEngineEvent::Multicast_ChangeText_Implementation(bool isactive)
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

void AEngineEvent::Multicast_PlayMusic_Implementation()
{
    if (pEngineMusic != nullptr)
    {
        pEngineAudio->FadeIn(25.0f);
    }
}

void AEngineEvent::Multicast_StopMusic_Implementation()
{
    if (pEngineMusic != nullptr)
    {
        pEngineAudio->FadeOut(4.0f, 0.0f);
    }
}

void AEngineEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->ActorHasTag("Player") && bIsActive == true && iBroughtItems == 0)
    {
        //Plays the anim when the player gets close
        if (pApproachAnim != nullptr)
        {
            Multicast_Animation(pDropOffMesh, pApproachAnim, false);
            GetWorld()->GetTimerManager().SetTimer(AnimTimer, this, &AEngineEvent::PlayIdleAnim, 1.0f);
        }
    }
    if (iBroughtItems < 1 && bIsActive == true)
    {
        if (OtherActor->ActorHasTag("EngineItem"))
        {
            APickUpItem* TempActor = Cast<APickUpItem>(OtherActor);
            TempActor->Deactivate();
            Multicast_ChangeStatus(iBroughtItems + iActivatedObjects);

            if (pLeaveAnim != nullptr)
            {
                Multicast_Animation(pDropOffMesh, pLeaveAnim, false);
                GetWorld()->GetTimerManager().SetTimer(AnimTimer, this, &AEngineEvent::PlayInsertionAnim, 1.0f);
            }
            iBroughtItems++;

            if (iActivatedObjects == 2 && iBroughtItems == 1)
            {
                bCanDeactivate = true;
            }
        }
    }
}

void AEngineEvent::OnColliderExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->ActorHasTag("Player") && bIsActive == true && iBroughtItems == 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(AnimTimer);
        if (pLeaveAnim != nullptr)
        {
            Multicast_Animation(pDropOffMesh, pLeaveAnim, false);
        }
    }
}

void AEngineEvent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEngineEvent, iBroughtItems);
    DOREPLIFETIME(AEngineEvent, iActivatedObjects);
    DOREPLIFETIME(AEngineEvent, pEventObjects);
    DOREPLIFETIME(AEngineEvent, pInnerMesh);
    DOREPLIFETIME(AEngineEvent, pCenterMesh);
    DOREPLIFETIME(AEngineEvent, pOuterMesh);
    DOREPLIFETIME(AEngineEvent, pDropOffMesh);
}