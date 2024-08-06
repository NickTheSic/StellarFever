// Copyright Quarantine Games 2020


#include "Shipshop.h"
#include "ShopItem.h"
#include "../../LevelProgression/RoundManager.h"
#include "../../../StellarFeverGameState.h"
#include "../../../Miscellaneous/ShipState.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "../../../Bay/AmmoBay.h"
#include "../../../Bay/MedicBay.h"
#include "../../PickupItems/VaccineObject.h"
#include "../../../Drops/AmmoDrops.h"
#include "../../../Drops/MedicDrops.h"
#include "../../SpawnPoint.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShipshop::AShipshop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creating the mesh and colider objets
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pMesh->SetIsReplicated(true);
	RootComponent = pMesh;

	//Creating the text block to house the currency amount
	pCashText = CreateDefaultSubobject<UTextRenderComponent>("TimerText");
	pCashText->SetIsReplicated(true);
	pCashText->SetupAttachment(RootComponent);

	bAlwaysRelevant = true;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void AShipshop::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		//Looping through all potential items and setitng the owner to this shop
		for (int i = 0; i < pShopItems.Num(); i++)
		{
			pShopItems[i]->SetShop(this);
		}
	}
}

// Called every frame
void AShipshop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShipshop::Activate()
{
	Server_Activate();
}

void AShipshop::Server_Activate_Implementation()
{
	//Sets the bool to true
	bIsActive = true;

	//Sets the text from the shipstate
	if (pShipState != nullptr)
	{
		SetCashText(pShipState->GetMoney());
	}

	//Displays the cash we have remaining
	pCashText->SetVisibility(true);

	//Sets the shop items to appear on the mesh, properly spaced i hope
	for (int i = 0; i < ucNumOfChoices; i++)
	{
		pPurchasableItems.Add(GetRandomShopItem());
		if (pPurchasableItems[i] != nullptr)
		{
			pPurchasableItems[i]->SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 200.0f - (i * 50.0f)));
			pPurchasableItems[i]->SetActorRotation(GetActorRotation());
			pPurchasableItems[i]->Activate();
			pPurchasableItems[i]->SetupShopText();
		}
	}
}

void AShipshop::Deactivate()
{
	Server_Deactivate();
}

void AShipshop::Server_Deactivate_Implementation()
{
	//Sets the bool to false
	bIsActive = false;

	//Removes functionality of the items just in case
	for (int i = 0; i < ucNumOfChoices; i++)
	{
		if (pPurchasableItems.IsValidIndex(i))
		{
			if (pPurchasableItems[i] != nullptr)
			{
				pPurchasableItems[i]->Disable();
			}
		}
	}

	//Clears the array and makes sure it is ready to be repopulated
	pPurchasableItems.Empty();
}

void AShipshop::PurchaseItem(AShopItem* item)
{
	Server_PurchaseItem(item);
}

void AShipshop::Server_PurchaseItem_Implementation(AShopItem* item)
{
	//Getting the type and checking if we have enough money to purchase the item
	ItemType itemtype = item->GetType();
	float cost = item->GetCost();

	//TODO: An else can be placed here to play an error sound if needed
	if (cost <= pShipState->GetMoney())
	{
		pShipState->SubMoney(cost);
		//Going through and finding out what item it is, because of enum it is obvious which is which
		switch (itemtype) {

		case Ammobay: {

			pAmmobay->PartialRefill(10);
			break;

		}

		case Medbay: {

			pMedbay->PartialRefill(200);
			break;

		}

		case Fuel: {

			pShipState->AddFuelAmount(200.0f);
			break;

		}

		case Vaccine: {

			pShipState->AddVaccines(1);

			FActorSpawnParameters SpawnParams;
			GetWorld()->SpawnActor<AVaccineObject>(pVaccineClass, pSpawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
			break;

		}

		case AmmoDrop: {

			FActorSpawnParameters SpawnParams;
			GetWorld()->SpawnActor<AAmmoDrops>(pAmmoDropClass, pSpawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
			break;

		}

		case MedDrop: {

			FActorSpawnParameters SpawnParams;
			GetWorld()->SpawnActor<AMedicDrops>(pMedicDropClass, pSpawnpoint->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
			break;

		}
		}
	}
}

void AShipshop::SetCashText(float currency)
{
	Multicast_SetCashText(currency);
}

AShopItem* AShipshop::GetRandomShopItem()
{
	if (pShopItems.Num() == 0)
		return nullptr;
	//loops through all items and gets a random one from the list
	int randval = FMath::RandRange(0, pShopItems.Num() - 1);
	AShopItem* temppointer = pShopItems[randval];

	//specific part of loop that just increments by one when enable to be used, and returns to 0 when too great a value
	while (temppointer->GetIsActive() == true)
	{
		randval++;
		if (randval > pShopItems.Num() - 1)
		{
			randval = 0;
		}
		temppointer = pShopItems[randval];
	}
	return temppointer;
}

void AShipshop::Multicast_SetCashText_Implementation(float currency)
{
	//Sets the Currency value so everyone can see
	if (currency > 0.0f)
	{
		pCashText->SetText(FText::FromString("You have " + FString::FromInt((int)currency) + ".0 Gloobuls"));
	}
	else
	{
		pCashText->SetText(FText::FromString("You Are Broke"));
	}
}

void AShipshop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShipshop, pCashText);
	DOREPLIFETIME(AShipshop, bIsActive);
	DOREPLIFETIME(AShipshop, pShopItems);
	DOREPLIFETIME(AShipshop, pPurchasableItems);
}