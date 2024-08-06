// Copyright Quarantine Games 2020


#include "RoomHealthEvent.h"
#include "../../Components/StatusComponent.h"

ARoomHealthEvent::ARoomHealthEvent()
{
    //Setting up the collision if a player wanders in
    pPlayerCollider = CreateDefaultSubobject<UBoxComponent>("PlayerCollider");
    pPlayerCollider->SetCollisionProfileName("OverlapAllDynamic");
    pPlayerCollider->SetupAttachment(RootComponent);

    //The overlap event when the player enters the collider
    pPlayerCollider->OnComponentBeginOverlap.AddDynamic(this, &ARoomHealthEvent::OnColliderOverlap);
    pPlayerCollider->OnComponentEndOverlap.AddDynamic(this, &ARoomHealthEvent::OnColliderExit);

    //Light source to indicate in the dark
    pInteractableMesh->SetMaterial(0, pGreenMaterial);

    Tags.Add("MinorEvent");
}

void ARoomHealthEvent::BeginPlay()
{
    Super::BeginPlay();
}

void ARoomHealthEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARoomHealthEvent::Activate()
{
    Super::Activate();

    //Sets the light to Red, explicitly shows where the interactable is
    pInteractableMesh->SetMaterial(0, pRedMaterial);
}

void ARoomHealthEvent::Deactivate()
{
    Super::Deactivate();

    //Sets the light to Red, explicitly shows where the interactable is
    pInteractableMesh->SetMaterial(0, pGreenMaterial);
}

void ARoomHealthEvent::LowerHealth()
{
    //Lowers all affected players health if they are within the collider
    for (int i = 0; i < pAffectedPlayers.Num(); i++)
    {
        pAffectedPlayers[i]->DrainHealth(fHealthDecrementValue);
    }
}

void ARoomHealthEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //Calls the player to lower the health on the player
    if (OtherActor->ActorHasTag("Player"))
    {
        ABasePlayer* pTempPlayer = Cast<ABasePlayer>(OtherActor);
        pAffectedPlayers.Add(pTempPlayer);
    }
}

void ARoomHealthEvent::OnColliderExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //cycles through to find the matching player and deletes them out of the array
    if (OtherActor->ActorHasTag("Player"))
    {
        ABasePlayer* pTempPlayer = Cast<ABasePlayer>(OtherActor);

        for (int i = 0; i < pAffectedPlayers.Num(); i++)
        {
            if (pTempPlayer == pAffectedPlayers[i])
            {
                pAffectedPlayers.RemoveAt(i);
            }
        }
    }
}
