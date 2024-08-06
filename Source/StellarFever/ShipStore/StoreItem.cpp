// Copyright Quarantine Games 2020


#include "StoreItem.h"
#include "../StellarFeverGameState.h"
#include "Kismet/GameplayStatics.h"
#include "../Miscellaneous/ShipState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AStoreItem::AStoreItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Item mesh");
	pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pMesh->SetIsReplicated(true);
	pMesh->SetupAttachment(RootComponent);

	this->SetReplicateMovement(true);
	this->SetReplicates(true);

}

void AStoreItem::Activate()
{
	Server_Activate();
}


void AStoreItem::Server_Activate_Implementation()
{
	//Allows vision of the item
	pMesh->SetVisibility(true);
}
void AStoreItem::Disable()
{
	Server_Disable();
}
void AStoreItem::Server_Disable_Implementation()
{
	//Disable vision of the item
	pMesh->SetVisibility(false);

}

// Called when the game starts or when spawned
void AStoreItem::BeginPlay()
{
	Super::BeginPlay();

	AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(UGameplayStatics::GetGameState(GetWorld()));
	pshipstate = gameState->GetShipState();
	
}

void AStoreItem::Purchase()
{
}

void AStoreItem::RandomizeAmount()
{
	iAmount = FMath::RandRange(iMinAmount, iMaxAmount);
}

void AStoreItem::RandomizeCost()
{
	iCost = FMath::RandRange(iMinCost, iMaxCost);
}

// Called every frame
void AStoreItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

