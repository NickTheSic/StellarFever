// Coded by Nick Lemmon

#include "../CaptainPlayer.h"
#include "../CaptainSpecial/CaptainSpecialScreen.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/TeleportActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACaptainPlayer::ACaptainPlayer()
{
	playerInformation.ePlayerClass = EPlayerClass::Captain;

	pCaptainScreen = CreateDefaultSubobject<UCaptainScreen>(TEXT("Captain Screen"));
	pCaptainScreen->SetupAttachment(RootComponent);

	pTeleportComponent = CreateDefaultSubobject<UTeleportActorComponent>(TEXT("Teleport Component"));

	//Hard coding it to twice the length of the Teleport Effect
	playerInformation.fActiveAbilityLength   = 2.0f;
	playerInformation.fActiveAbilityCooldown = 120.0f;

	Tags.Add("Captain");
}


void ACaptainPlayer::SetTeleportInformation(int idx)
{
	iCaptainTeleportInformation = idx;
	Server_SetTeleportInformation(idx);
}

void ACaptainPlayer::Server_SetTeleportInformation_Implementation(int idx)
{
	iCaptainTeleportInformation = idx;
}


void ACaptainPlayer::StartRageMode()
{
	//Lock the inventory to only the melee
	SetCanAccessInventory(false);
	//Increase and lock the captains speed
	AddToPlayerStatusFlag(EPlayerStatus::Enraged);
}

void ACaptainPlayer::EndRageMode()
{
	//Unlock the inventory
	SetCanAccessInventory(true);
	//Reset speed to normal
	RemoveFromPlayerStatusFlag(EPlayerStatus::Enraged);
}


void ACaptainPlayer::BeginPlay()
{
	Super::BeginPlay();


	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Teleportable", targetSpots);
	pCaptainScreen->SetButtonCount(targetSpots.Num()); //Do this after we get all the target spots
	pCaptainScreen->DeactivateSpecialMenu();
	
	LeaveCaptainSpecialMenu(); //A quick and easy way to make sure we start the game not in the menu
}


bool ACaptainPlayer::UseSpecial()
{
	//If the captain is on the enemy ship we don't want him to be able to teleport back to the player ship
	if (GetIsOnEnemyShip() || GetIsEnraged())
	{
		if (GetIsPlayerInMenu())
		{
			LeaveCaptainSpecialMenu();
		}

		return false;
	}

	
	switch (iCaptainTeleportInformation)
	{
	case -2:	//The player chose to leave the menu
		LeaveCaptainSpecialMenu();
		return false;

	case -1:	//We pressed Q for the first time and open the menu
		EnterCaptainSpecialMenu();
		return false;

	default:	//We have selected a spot that we want to teleport too
		//We have to assume if they got here that it was true
		TeleportTheCaptain();
		return true;
	}
}

void ACaptainPlayer::LeaveCaptainSpecialMenu()
{
	//Player cancelled
	RemoveFromPlayerStatusFlag(EPlayerStatus::PlayerInMenu);

	//Reset iCaptainTeleportInformation to -1 
	iCaptainTeleportInformation = -1;

	//Take away menu and let the player continue as normal
	pCaptainScreen->DeactivateSpecialMenu();

	NetMulticast_LeaveCaptainSpecialMenu();
}

void ACaptainPlayer::NetMulticast_LeaveCaptainSpecialMenu_Implementation()
{
	pCaptainScreen->DeactivateSpecialMenu();

	//Reset iCaptainTeleportInformation to -1 
	iCaptainTeleportInformation = -1;
}



void ACaptainPlayer::EnterCaptainSpecialMenu()
{
	//Player has pressed Q for the first time
	AddToPlayerStatusFlag(EPlayerStatus::PlayerInMenu);

	//Bring up the interaction menu
	pCaptainScreen->ActivateSpecialMenu();

	//Set it to -2 so if we press Q again it dissapears
	iCaptainTeleportInformation = -2;

	NetMulticast_EnterCaptainSpecialMenu();
}

void ACaptainPlayer::NetMulticast_EnterCaptainSpecialMenu_Implementation()
{
	pCaptainScreen->ActivateSpecialMenu();

	//Set it to -2 so if we press Q again it dissapears
	iCaptainTeleportInformation = -2;
}


void ACaptainPlayer::TeleportTheCaptain()
{
	Server_TeleportTheCaptain();

	//Leave the captain special menu
	LeaveCaptainSpecialMenu();
}

void ACaptainPlayer::Server_TeleportTheCaptain_Implementation()
{
	//The value was 0 or greater - the player probably wants to teleport
	//Save the value in an index
	int index = iCaptainTeleportInformation;
	iCaptainTeleportInformation = -1;

	//Check if it is a valid teleport spot
	if (targetSpots.IsValidIndex(index))
	{
		FVector loc = targetSpots[index]->GetActorLocation();
		FRotator rot = targetSpots[index]->GetActorRotation();

		pTeleportComponent->StartTeleporting(loc, rot);
	}
}


void ACaptainPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptainPlayer, iCaptainTeleportInformation);
}
