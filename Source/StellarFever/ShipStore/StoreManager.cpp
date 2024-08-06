// Copyright Quarantine Games 2020


#include "StoreManager.h"
#include "../ShipStore/StoreItem.h"
#include "../ShipStore/StoreMenu.h"
#include "Net/UnrealNetwork.h"
#include "StoreMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "../Miscellaneous/ShipState.h"
#include "../StellarFeverGameState.h"

// Sets default values
AStoreManager::AStoreManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>("medic Collider");
	pMonitorMesh->SetCollisionProfileName("BlockAll");
	RootComponent = pMonitorMesh;

	pMenuWidget = CreateDefaultSubobject<UStoreMenu>("Store Menu");
	pMenuWidget->SetIsReplicated(true);
	pMenuWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	this->SetReplicateMovement(true);
	this->SetReplicates(true);

	Tags.Add("Store");
}

void AStoreManager::BeginPlay()
{
	Super::BeginPlay();

	//get array size of items available
	if (GetLocalRole() == ROLE_Authority)
	{
		AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(UGameplayStatics::GetGameState(GetWorld()));
		pshipstate = gameState->GetShipState();
	}
	//pass in array of numbers
	pMenuWidget->SetButtonCount(iItemAmount); //Do this after we get all the target spots

	//CloseStoreMenu();
	OpenStoreMenu();
}

void AStoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStoreManager::SetPurchaseInformation(int idx)
{
	iPuchaseInformation = idx;
	Server_SetPurchaseInformation(idx);
}

void AStoreManager::Server_SetPurchaseInformation_Implementation(int idx)
{
	iPuchaseInformation = idx;
}

// Called when the game starts or when spawned

void AStoreManager::PurchaseAvailableItems()
{
	switch (iPuchaseInformation)
	{
	case -2:	//The player chose to leave the menu
		CloseStoreMenu();
		break;

	case -1:	//We pressed Q for the first time and open the menu
		OpenStoreMenu();
		break;

	default:	//We have selected a spot that we want to teleport too
		//We have to assume if they got here that it was true
		PurchaseTheItem();
	}
}

void AStoreManager::CloseStoreMenu()
{
	//Reset iCaptainTeleportInformation to -1 
	iPuchaseInformation = -1;

	//Take away menu and let the player continue as normal
	pMenuWidget->DeactivateStoreMenu();

	NetMulticast_CloseStoreMenu();
}

void AStoreManager::NetMulticast_CloseStoreMenu_Implementation()
{
	pMenuWidget->DeactivateStoreMenu();

	//Reset iPuchaseInformation to -1 
	iPuchaseInformation = -1;
}

// Called every frame

void AStoreManager::OpenStoreMenu()
{
	//Bring up the store menu
	pMenuWidget->ActivateStoreMenu();

	//Set it to -2 so if we press Q again it dissapears
	iPuchaseInformation = -2;

	NetMulticast_OpenStoreMenu();
}

void AStoreManager::NetMulticast_OpenStoreMenu_Implementation()
{
	pMenuWidget->ActivateStoreMenu();

	//Set it to -2 so if we press Q again it dissapears
	iPuchaseInformation = -2;
}


void AStoreManager::PurchaseTheItem()
{
	Server_PurchaseTheItem();
}

void AStoreManager::Server_PurchaseTheItem_Implementation()
{
	//The value was 0 or greater - the player probably wants to purchase
	//Save the value in an index
	int index = iPuchaseInformation;
	iPuchaseInformation = -1;

	//Check if it is a valid purchase
	if (Items.IsValidIndex(index) == true)
	{
		AStoreItem* item = Items[index]->GetDefaultObject<AStoreItem>();
		item->SetShipState(pshipstate);
		item->Purchase();
	}
}

void AStoreManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStoreManager, iPuchaseInformation);
	DOREPLIFETIME(AStoreManager, pMenuWidget);
}