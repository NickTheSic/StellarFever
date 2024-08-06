// Copyright Quarantine Games 2020


#include "RoomOxygenEvent.h"
#include "../../Components/StatusComponent.h"

ARoomOxygenEvent::ARoomOxygenEvent()
{
    //Setting up the collision if a player wanders in
    pPlayerCollider = CreateDefaultSubobject<UBoxComponent>("PlayerCollider");
    pPlayerCollider->SetCollisionProfileName("OverlapAllDynamic");
    pPlayerCollider->SetupAttachment(RootComponent);

    //The overlap event when the player enters the collider
    pPlayerCollider->OnComponentBeginOverlap.AddDynamic(this, &ARoomOxygenEvent::OnColliderOverlap);
    pPlayerCollider->OnComponentEndOverlap.AddDynamic(this, &ARoomOxygenEvent::OnColliderExit);

    //Light source to indicate in the dark
    pInteractableMesh->SetMaterial(0, pGreenMaterial);

    Tags.Add("MinorEvent");
}

void ARoomOxygenEvent::BeginPlay()
{
    Super::BeginPlay();

    Activate();
}

void ARoomOxygenEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARoomOxygenEvent::Activate()
{
    Super::Activate();

    //Sets the light to Red, explicitly shows where the interactable is
    pInteractableMesh->SetMaterial(0, pRedMaterial);

    //Sets up the timer to lower the max oxygen
    GetWorld()->GetTimerManager().SetTimer(MaxOxygenDecrementTimer, this, &ARoomOxygenEvent::LowerMaxOxygen, fMaxOxygenDecrementSpeed, true);
    GetWorld()->GetTimerManager().SetTimer(OxygenDecrementTimer, this, &ARoomOxygenEvent::LowerOxygen, fOxygenDecrementSpeed, true);
}

void ARoomOxygenEvent::Deactivate()
{
    Super::Deactivate();

    //Sets the light to Green, explicitly shows that the event is finished
    pInteractableMesh->SetMaterial(0, pGreenMaterial);

    //Clears the timer so it stops calling the lowermaxoxygen function
    GetWorld()->GetTimerManager().ClearTimer(OxygenDecrementTimer);
    GetWorld()->GetTimerManager().ClearTimer(MaxOxygenDecrementTimer);
}

void ARoomOxygenEvent::LowerOxygen()
{
    //lowers the player's oxygen value
    for (int i = 0; i < pAffectedPlayers.Num(); i++)
    {
        pAffectedPlayers[i]->DrainOxygen(fOxygenDecrementValue);
    }

}

void ARoomOxygenEvent::LowerMaxOxygen()
{
    //lowers the player's max oxygen values on all players
    for (int i = 0; i < pPlayers.Num(); i++)
    {
        float ftempval = pPlayers[i]->GetOxygenComponent()->GetMaxValue();
        ftempval--;
        pPlayers[i]->GetOxygenComponent()->SetMaxValue(ftempval);
    }
}

void ARoomOxygenEvent::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //Calls the player to lower the oxygen on the player
    if (OtherActor->ActorHasTag("Player"))
    {
        ABasePlayer* pTempPlayer = Cast<ABasePlayer>(OtherActor);
        pAffectedPlayers.Add(pTempPlayer);
    }
}

void ARoomOxygenEvent::OnColliderExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
