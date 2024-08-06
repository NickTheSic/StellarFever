// Copyright Quarantine Games 2020


#include "StoreMenu.h"
#include "StoreMenuWidget.h"
#include "StoreManager.h"
#include "../Miscellaneous/ShipState.h"
#include "../StellarFeverGameState.h"

UStoreMenu::UStoreMenu()
{
	//Setting up default values that worked at first
	GeometryMode = EWidgetGeometryMode::Plane;
	bIsTwoSided = true;
	SetCollisionProfileName("NoCollision");
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void UStoreMenu::ActivateStoreMenu()
{
	//Activate self
	SetActive(true);
	SetVisibility(true);
	SetCollisionProfileName("UI");
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void UStoreMenu::DeactivateStoreMenu()
{
	//Deactive self
	SetActive(false);
	SetVisibility(false);
	SetCollisionProfileName("NoCollision");
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void UStoreMenu::SetToPurchase(int8 PurchaseInfo)
{
	if (pStoreManager != nullptr)
	{
		pStoreManager->SetPurchaseInformation(PurchaseInfo);
		pStoreManager->PurchaseAvailableItems();
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "The Player was null inside the screen");
}

void UStoreMenu::BeginPlay()
{
	Super::BeginPlay();

	pStoreManager = Cast<AStoreManager>(GetOwner());
	if (pStoreManager == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This wasn't attached to a monitor"));
	}

	pMenuWidget = Cast<UStoreMenuWidget>(GetUserWidgetObject());
	if (pMenuWidget != nullptr)
	{
		pMenuWidget->SetStoreManager(pStoreManager);
		pMenuWidget->SetStoreMenu(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The store monitor widget was null"));
	}
}


void UStoreMenu::SetButtonCount(int32 numButtons)
{
	if (pMenuWidget != nullptr)
	{
		pMenuWidget->CreateButtons(numButtons);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("pSpecialWidget was Null when this was called"));
}
