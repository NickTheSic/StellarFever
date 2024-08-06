// Copyright Quarantine Games 2020


#include "TutorialManager.h"
#include "../Ship parts/ShipDoor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "../StellarFeverGameModeBase.h"
#include "../Events/EventsManager.h"
#include "../Events/EnemyPool.h"
#include "../AI/AIEnemy.h"
#include "../StellarFeverGameInstance.h"
#include "../Player/BasePlayer.h"
#include "../Components/InventoryComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/StatusComponent.h"

// Sets default values
ATutorialManager::ATutorialManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fMinHealth = 20.0f;
	fMinOxygen = 20.0f;

	pAudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComp");

	bAmmoBay = false;
	bMedBay = false;
	bBaysDone = false;
	bTutorialFinished = false;
}

// Called when the game starts or when spawned
void ATutorialManager::BeginPlay()
{
	Super::BeginPlay();
	
	bEnginesDown = false;
	bHPOxygenDown = false;

	//Lock all doors by default
	LockDoors();

	//get player
	for (TActorIterator<ABasePlayer> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr)
		{
			pPlayer = *ActorItr;
			//disable inventory
			pPlayer->GetInventoryComponent()->DisableInventory();
		}
	}

	//make audio component use audio manager thing for volume settings
	pAudioComponent->SoundClassOverride = GetGameInstance<UStellarFeverGameInstance>()->pVoiceSoundClass;
	PlayGreeting();
}

// Called every frame
void ATutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if we've hit ammo bay and med bay
	if (bAmmoBay && bMedBay && bBaysDone == false)
	{
		//tell player to return to the bridge
		PlayReturnToBridge();
		bBaysDone = true;
	}

	//if player isn't supposed to get inventory yet, and their inventory is activated (happens when you grab a pickup)
	if (bHPOxygenDown == false && pPlayer->GetInventoryComponent()->CanAccessInventory() == true)
	{
		pPlayer->GetInventoryComponent()->DisableInventory();
	}

	//if we're done the tutorial
	if (bTutorialFinished)
	{
		//and we're not playing audio
		if (!pAudioComponent->IsPlaying())
		{
			//load main menu
			UGameplayStatics::OpenLevel(this, TEXT("MainMenuMap"));
		}
	}

	//STOPS Player from dying (might get ruined by burst damage?)
	if (pPlayer)
	{
		//if player's health is less than the min health
		if (pPlayer->GetHealthComponent()->GetHealth() < fMinHealth)
		{
			//set health to min health
			pPlayer->GetHealthComponent()->TutorialSetHealth(fMinHealth);
		}

		//if player's oxygen is less than the min oxygen
		if (pPlayer->GetOxygenComponent()->GetValue() < fMinOxygen)
		{
			//set oxygen to min oxygen
			pPlayer->GetOxygenComponent()->SetValue(fMinOxygen);
		}
	}
}

void ATutorialManager::SpawnAI()
{
	AStellarFeverGameModeBase* gameMode = Cast<AStellarFeverGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode)
	{
		//Ranged
		Enemies.Add(gameMode->pEventManager->GetEnemyPool()->SpawnOneEnemy(3, EnemySpawnPoints[0]));
		Enemies[0]->ShipBoardedActivate();
		//Melee to Ranged
		Enemies.Add(gameMode->pEventManager->GetEnemyPool()->SpawnOneEnemy(3, EnemySpawnPoints[1]));
		Enemies[1]->ShipBoardedActivate();
		Enemies[1]->ChasePlayerActivate();
		//Shield to Shotgun
		Enemies.Add(gameMode->pEventManager->GetEnemyPool()->SpawnOneEnemy(6, EnemySpawnPoints[2]));
		Enemies[2]->ShipBoardedActivate();
		Enemies[2]->ChasePlayerActivate();
		//Shotgun
		Enemies.Add(gameMode->pEventManager->GetEnemyPool()->SpawnOneEnemy(6, EnemySpawnPoints[3]));
		Enemies[3]->ShipBoardedActivate();
		Enemies[3]->ChasePlayerActivate();
		//Exploder
		Enemies.Add(gameMode->pEventManager->GetEnemyPool()->SpawnOneEnemy(4, EnemySpawnPoints[4]));
		Enemies[4]->ShipBoardedActivate();
		Enemies[4]->ChasePlayerActivate();
	}
}

void ATutorialManager::LockDoors()
{
	//lock all ship doors
	for (auto door : ShipDoors)
	{
		door->HackedDoor();
	}
}

void ATutorialManager::UnlockDoor(AShipDoor* door)
{
	door->FixDoor();
}

void ATutorialManager::PlayGreeting()
{
	pAudioComponent->SetSound(SoundCues[0]);
	pAudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &ATutorialManager::PlayGreeting_Repeat, 60.0f, true); //call this function again after 30s
}

void ATutorialManager::PlayEngineRoom()
{
	pAudioComponent->SetSound(SoundCues[2]);
	pAudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &ATutorialManager::PlayEngineRoom_Repeat, 60.0f, true); //call this function again after 30s
	
}

void ATutorialManager::PlayEnemiesAndSystems()
{
	pAudioComponent->SetSound(SoundCues[4]);
	pAudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &ATutorialManager::PlayEnemiesAndSystems_Repeat, 60.0f, true); //call this function again after 30s
}

void ATutorialManager::PlayGetHealthAndAmmo()
{
	pAudioComponent->SetSound(SoundCues[6]);
	pAudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &ATutorialManager::PlayGetHealthAndAmmo_Repeat, 60.0f, true); //call this function again after 30s
}

void ATutorialManager::PlayReturnToBridge()
{
	pAudioComponent->SetSound(SoundCues[8]);
	pAudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &ATutorialManager::PlayReturnToBridge_Repeat, 60.0f, true); //call this function again after 30s
}

void ATutorialManager::PlayFarewell()
{
	GetWorld()->GetTimerManager().ClearTimer(TutorialTimer); //SetTimer calls ClearTimer anyway so we only need this function to clear since the others will already do it
	pAudioComponent->SetSound(SoundCues[10]);
	pAudioComponent->Play();
	bTutorialFinished = true;
	//set tutorial to be complete and then save game
	GetGameInstance<UStellarFeverGameInstance>()->bTutorialComplete = true;
	GetGameInstance<UStellarFeverGameInstance>()->SaveGame();
}

void ATutorialManager::ShutDownEngines()
{
	for (int i = 0; i < 5; i++)
	{
		ShipDoors[i]->FixDoor();
	}
	bEnginesDown = true;
	PlayEngineRoom();
}

void ATutorialManager::ShutDownHealthOxygen()
{
	SpawnAI();
	UnlockDoor(ShipDoors[5]);
	UnlockDoor(ShipDoors[6]);
	bHPOxygenDown = true;
	PlayEnemiesAndSystems();

	//Re enable inventory 
	pPlayer->GetInventoryComponent()->EnableInventory();
}

void ATutorialManager::UnlockArmory()
{
	bArmoryUnlocked = true;
	//unlock armory door
	UnlockDoor(ShipDoors[7]);
	//tell player to get health and ammo
	PlayGetHealthAndAmmo();
}