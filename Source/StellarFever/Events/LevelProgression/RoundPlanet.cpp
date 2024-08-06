// Copyright Quarantine Games 2020


#include "RoundPlanet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "RoundManager.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARoundPlanet::ARoundPlanet()
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

	pEventText = CreateDefaultSubobject<UTextRenderComponent>("EventText");
	pEventText->SetIsReplicated(true);
	pEventText->SetupAttachment(RootComponent);

	pFuelText = CreateDefaultSubobject<UTextRenderComponent>("FuelText");
	pFuelText->SetIsReplicated(true);
	pFuelText->SetupAttachment(RootComponent);

	this->SetReplicateMovement(true);
	this->SetReplicates(true);

	bisActive = false;
	bisUsed = false;

	Tags.Add("Planet");
}

// Called when the game starts or when spawned
void ARoundPlanet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARoundPlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FName ARoundPlanet::CreateadditionalText(FName event)
{
//#if WITH_EDITOR
//	return event;
//#endif

	if (event == FName("Bathroom"))
	{
		return FName("Heavy toxic<br>gas in<br>orbit.");
	}
	if (event == FName("Boarding"))
	{
		return FName("Very active<br>and violent<br>locals.");
	}
	if (event == FName("Bridge"))
	{
		return FName("Highly magnetic<br>orbit and lunar<br>bodies.");
	}
	if (event == FName("Depressure"))
	{
		return FName("Deadly<br>electromagnetic field<br>in orbit.");
	}
	if (event == FName("EnemyShip"))
	{
		return FName("Violent vandals<br>in orbit.");
	}
	if (event == FName("Engine"))
	{
		return FName("Thick nebulas<br>en route.");
	}
	if (event == FName("Greenhouse"))
	{
		return FName("Flammable gas<br>in orbit");
	}
	if (event == FName("LifeSupport"))
	{
		return FName("Potentially lethal<br>parasite detected.");
	}
	if (event == FName("OxygenSupply"))
	{
		return FName("Bio-debris<br>detected in<br>orbit.");
	}
	if (event == FName("PlayerShield"))
	{
		return FName("Heavy static<br>field detected<br>in orbit.");
	}
	if (event == FName("Power"))
	{
		return FName("Power surges<br>near destination");
	}
	
	//a backup incase it goes kaput
	return event;
}

void ARoundPlanet::Activate()
{
	Server_Activate();
	Multicast_SetupPlanetText();
}

void ARoundPlanet::Server_Activate_Implementation()
{
	//Allows vision of the planet
	pMesh->SetVisibility(true);
	bisActive = true;

	//Making sure the collision is properly set
	pCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ARoundPlanet::Deactivate()
{
	Server_Deactivate();
}

void ARoundPlanet::Server_Deactivate_Implementation()
{
	//removes the planet from visibility
	pMesh->SetVisibility(false);
	bisActive = false;

	//Activates the proper level in the roundmanager using THIS planet
	pRoundManager->StartLevel(this);
	pRoundManager->Deactivate();

	//removes the usability of the planet
	bisUsed = true;
}

void ARoundPlanet::Disable()
{
	Server_Disable();
}

void ARoundPlanet::SetupPlanetText()
{
	Multicast_SetupPlanetText();
}

void ARoundPlanet::Multicast_SetupPlanetText_Implementation()
{
	//Sets the planets name to the text
	FText planetname = FText::FromName(PlanetName);
	pNameText->SetText(planetname);

	//Sets the fuel cost to the text
	FText time = FText::AsNumber(fRoundTime);
	pFuelText->SetText(time);

	//Sets the event name to the text
	FText eventname = FText::FromName(CreateadditionalText(EventName));
	pEventText->SetText(eventname);
}

void ARoundPlanet::Server_Disable_Implementation()
{
	//removes the planet from visibility
	pMesh->SetVisibility(false);
	bisActive = false;

	//A Safety check as if the collision remains players can crash the game accessing null
	pCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Server_DeactivateInfo();
}

void ARoundPlanet::ActivateInfo()
{
	Server_ActivateInfo();
}

void ARoundPlanet::Server_ActivateInfo_Implementation()
{
	//Sets the texts visibility to true and visible
	pNameText->SetVisibility(true);
	pEventText->SetVisibility(true);
	pFuelText->SetVisibility(true);
}

void ARoundPlanet::DeactivateInfo()
{
	Server_DeactivateInfo();
}

void ARoundPlanet::Server_DeactivateInfo_Implementation()
{
	//Sets the texts visibility to false and invisible
	pNameText->SetVisibility(false);
	pEventText->SetVisibility(false);
	pFuelText->SetVisibility(false);
}

void ARoundPlanet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARoundPlanet, PlanetName);
	DOREPLIFETIME(ARoundPlanet, EventName);
	DOREPLIFETIME(ARoundPlanet, fFuelCost);
	DOREPLIFETIME(ARoundPlanet, pNameText);
	DOREPLIFETIME(ARoundPlanet, pEventText);
	DOREPLIFETIME(ARoundPlanet, pFuelText);
}