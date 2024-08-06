// Copyright Quarantine Games 2020


#include "ShopItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Shipshop.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShopItem::AShopItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pCollider = CreateDefaultSubobject<UBoxComponent>("MainCollider");
	pCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pCollider->SetCollisionProfileName("BlockAll");
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	RootComponent = pCollider;

	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("MainMesh");
	pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pMesh->SetIsReplicated(true);
	pMesh->SetupAttachment(RootComponent);

	//Setting up all the texts below
	pNameText = CreateDefaultSubobject<UTextRenderComponent>("NameText");
	pNameText->SetIsReplicated(true);
	pNameText->SetupAttachment(RootComponent);

	//Setting up all the texts below
	pCostText = CreateDefaultSubobject<UTextRenderComponent>("CostText");
	pCostText->SetIsReplicated(true);
	pCostText->SetupAttachment(RootComponent);

	this->SetReplicateMovement(true);
	this->SetReplicates(true);

	bIsActive = false;

	Tags.Add("Shop");
}

// Called when the game starts or when spawned
void AShopItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShopItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShopItem::Activate()
{
	Server_Activate();
}

void AShopItem::Server_Activate_Implementation()
{
	//Sets up what the cost of the item is
	fCost = (float)FMath::RandRange(ucMinCost, ucMaxCost);

	//Allows vision of the item
	pMesh->SetVisibility(true);
	bIsActive = true;

	//Making sure the collision is properly set
	pCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ActivateInfo();
}

void AShopItem::Deactivate()
{
	Server_Deactivate();
}

void AShopItem::Server_Deactivate_Implementation()
{
	//Used for when the item is interacted with, all logic is dealt by the shop itself

	//Tells the shop that this item specifically was purchased
	pShop->PurchaseItem(this);

}

void AShopItem::Disable()
{
	Server_Disable();
}

void AShopItem::ActivateInfo()
{
	Server_ActivateInfo();
}

void AShopItem::Server_ActivateInfo_Implementation()
{
	//Sets the texts visibility to true and visible
	pNameText->SetVisibility(true);
	pCostText->SetVisibility(true);
}

void AShopItem::DeactivateInfo()
{
	Server_DeactivateInfo();
}

void AShopItem::Server_DeactivateInfo_Implementation()
{
	//Sets the texts visibility to true and visible
	pNameText->SetVisibility(false);
	pCostText->SetVisibility(false);
}

void AShopItem::SetupShopText()
{
	Multicast_SetupShopText();
}

void AShopItem::Multicast_SetupShopText_Implementation()
{
	//ensuring all values are as Ftext to be used onto the UI
	FString item = UEnum::GetValueAsString(itemtype);
	FText itemtext = FText::FromString(item);

	//Assigning all texts to the values given
	pNameText->SetText(itemtext);
	pCostText->SetText(FText::FromString("Costs: " + FString::FromInt(fCost)));
}

void AShopItem::Server_Disable_Implementation()
{
	//removes the planet from visibility
	pMesh->SetVisibility(false);
	bIsActive = false;

	//A Safety check as if the collision remains players can crash the game accessing null
	pCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DeactivateInfo();
}

void AShopItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShopItem, bIsActive);
	DOREPLIFETIME(AShopItem, itemtype);
	DOREPLIFETIME(AShopItem, fCost);
	DOREPLIFETIME(AShopItem, pNameText);
	DOREPLIFETIME(AShopItem, pCostText);
}
