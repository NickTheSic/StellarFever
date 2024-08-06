// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/


#include "CaptainSpecialScreen.h"
#include "CaptainSpecialWidget.h"

UCaptainScreen::UCaptainScreen()
{
	//Setting up default values that worked at first
	GeometryMode = EWidgetGeometryMode::Cylinder;
	bIsTwoSided = true;
	SetCollisionProfileName("NoCollision");
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void UCaptainScreen::ActivateSpecialMenu()
{
	//Activate self
	SetActive(true);
	SetVisibility(true);
	SetCollisionProfileName("UI");
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void UCaptainScreen::DeactivateSpecialMenu()
{
	//Deactive self
	SetActive(false);
	SetVisibility(false);
	SetCollisionProfileName("NoCollision");
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void UCaptainScreen::SetCaptainToTeleport(int8 teleportInfo)
{
	if (pCaptainPlayer != nullptr)
	{
		pCaptainPlayer->SetTeleportInformation(teleportInfo);
		pCaptainPlayer->ClassSpecial();
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "The captain was null inside the screen");
}

void UCaptainScreen::BeginPlay()
{
	Super::BeginPlay();

	pCaptainPlayer = Cast<ACaptainPlayer>(GetOwner());
	if (pCaptainPlayer == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This wasn't attached to a Captain player"));
	}

	pSpecialWidget = Cast<UCaptainSpecialWidget>(GetUserWidgetObject());
	if (pSpecialWidget != nullptr)
	{
		pSpecialWidget->SetCaptainPlayer(pCaptainPlayer);
		pSpecialWidget->SetCaptainScreen(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The Captain Special widget was null"));
	}
}

void UCaptainScreen::SetButtonCount(int32 numButtons)
{
	if (pSpecialWidget != nullptr)
	{
		pSpecialWidget->CreateButtons(numButtons);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("pSpecialWidget was Null when this was called"));
}

