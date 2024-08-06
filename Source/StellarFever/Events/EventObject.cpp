// Copyright Quarantine Games 2020


#include "EventObject.h"
#include "Net/UnrealNetwork.h"

AEventObject::AEventObject()
{
    //Setting up the skeletal mesh basic stuff
    pButtonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ButtonMesh");
    pButtonMesh->SetCollisionProfileName("BlockAll");
    pButtonMesh->bRenderCustomDepth = true;
    pButtonMesh->SetupAttachment(RootComponent);

    pEventOwner = nullptr;
    bIsUseable = true;

    SetReplicates(true);
    bAlwaysRelevant = true;
}

void AEventObject::BeginPlay()
{
    Super::BeginPlay();
}

void AEventObject::Tick(float DeltaTime)
{
    APlayerController* buttonctrl = GEngine->GetFirstLocalPlayerController(GetWorld());
    if (buttonctrl != nullptr && buttonctrl->GetPawn() != nullptr && bForTeleporter == false)
    {
        if (pButtonMesh != nullptr)
        {
            if (pButtonMesh->GetMaterial(1) == pRedMaterial && FVector::Distance(GetActorLocation(), buttonctrl->GetPawn()->GetActorLocation()) <= fDistanceForRenderOutline)
            {
                pButtonMesh->SetCustomDepthStencilValue(1);
            }
            else
            {
                pButtonMesh->SetCustomDepthStencilValue(0);
            }
        }
    }

    Super::Tick(DeltaTime);
}

void AEventObject::Activate()
{
    Super::Activate();
}

void AEventObject::Server_Activate_Implementation()
{
    Super::Server_Activate_Implementation();
}

void AEventObject::Multicast_Activate_Implementation()
{
    Super::Multicast_Activate_Implementation();

    //Sets the light to red
    pButtonMesh->SetMaterial(1, pRedMaterial);
}

void AEventObject::Deactivate()
{
    Super::Deactivate();
}

void AEventObject::Server_Deactivate_Implementation()
{
    //Ensures that the owner of the event object knows it is deactivated
    if (pEventOwner != nullptr)
    {
        if (bIsUseable == true)
        {
            CallComplete();
        }
        else
        {
            Failure();
        }
    }

    //Plays the buttons anim
    if (pButtonAnim != nullptr && bIsActive == true)
    {
        Multicast_Animation(pButtonMesh, pButtonAnim, false);
    }

    Super::Server_Deactivate_Implementation();
}

void AEventObject::Multicast_Deactivate_Implementation()
{
    Super::Multicast_Deactivate_Implementation();

    //Sets the light to red
    pButtonMesh->SetMaterial(1, pGreenMaterial);
}

void AEventObject::Multicast_Animation_Implementation(USkeletalMeshComponent* mesh, UAnimationAsset* anim, bool loop)
{
    //Plays the animation given and sees if it loops or not
    mesh->PlayAnimation(anim, loop);
}

void AEventObject::Failure()
{
    pEventOwner->Failure();
}

void AEventObject::CallComplete()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (pEventOwner != nullptr)
        {
            pEventOwner->EventObjectComplete();
        }
    }
}

void AEventObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEventObject, bIsUseable);
    DOREPLIFETIME(AEventObject, pButtonMesh);
}

