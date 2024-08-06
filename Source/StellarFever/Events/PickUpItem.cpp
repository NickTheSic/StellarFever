// Copyright Quarantine Games 2020


#include "PickUpItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting up the collider
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pMesh->SetCollisionProfileName("BlockAll");
	pMesh->SetSimulatePhysics(true);
	pMesh->SetIsReplicated(true);
	RootComponent = pMesh;

	Tags.Add("PickUpItem");

	SetReplicates(true);
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	APlayerController* ctrl = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (ctrl != nullptr && ctrl->GetPawn() != nullptr)
	{
		if (pMesh != nullptr)
		{
			if (GetOwner() == nullptr && FVector::Distance(GetActorLocation(), ctrl->GetPawn()->GetActorLocation()) <= fDistanceForRenderOutline)
			{
				pMesh->SetCustomDepthStencilValue(1);
			}
			else
			{
				pMesh->SetCustomDepthStencilValue(0);
			}
		}
	}
}

void APickUpItem::Activate()
{
	Server_Activate();
}

void APickUpItem::Multicast_Activate_Implementation()
{

}

void APickUpItem::Server_Activate_Implementation()
{
	//Turns on the object to be visible
	pMesh->SetCollisionProfileName("BlockAll");
	pMesh->SetVisibility(true);
	pMesh->SetSimulatePhysics(true);
	bIsActive = true;
}

void APickUpItem::Deactivate()
{
	Server_Deactivate();
}

void APickUpItem::Multicast_Deactivate_Implementation()
{

}

void APickUpItem::Server_Deactivate_Implementation()
{
	//Turns off the object so it cant be interacted with or seen
	pMesh->SetVisibility(false);
	pMesh->SetSimulatePhysics(false);
	pMesh->SetCollisionProfileName("NoCollision");
	bIsActive = false;
}

void APickUpItem::PickUpObject()
{
	Multicast_PickUpObject();
}

void APickUpItem::Multicast_PickUpObject_Implementation()
{
	pMesh->SetSimulatePhysics(false);
	pMesh->SetCollisionProfileName("NoCollision");
}

void APickUpItem::DropObject(FVector direction)
{
	Multicast_DropObject(direction);
}

void APickUpItem::Multicast_DropObject_Implementation(FVector direction)
{
	pMesh->SetCollisionProfileName("BlockAll");
	pMesh->SetSimulatePhysics(true);
	pMesh->AddImpulse(direction * fThrowSpeed);
}

void APickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickUpItem, bIsActive);
	DOREPLIFETIME(APickUpItem, pMesh);
}

