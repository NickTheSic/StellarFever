// Copyright Quarantine Games 2020


#include "TutorialManager2.h"
#include "../Ship parts/ShipDoor.h"
#include "../AI/ScavengerAI.h"
#include "../AI/AIEnemy.h"
#include "../StellarFeverGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Events/EventsManager.h"
#include "../Events/EnemyPool.h"
#include "../Events/SpawnPoint.h"
#include "../Events/EventObject.h"
#include "../Events/BaseEvent.h"
#include "../Events/PickupItems/VaccineObject.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "../StellarFeverGameInstance.h"
#include "../Player/BasePlayer.h"
#include "../Components/InventoryComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/StatusComponent.h"

// Sets default values
ATutorialManager2::ATutorialManager2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pAudioComponent = CreateDefaultSubobject<UAudioComponent>("Tutorial Sound Player");
	pAudioComponent->OnAudioFinished.AddDynamic(this, &ATutorialManager2::OnAudioFinished);
	ProgressionTracker = 0;

}

// Called when the game starts or when spawned
void ATutorialManager2::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < ShipDoors.Num(); i++)
	{
		ShipDoors[i]->HackedDoor();
	}

	PlayMovement();
	TeleporterWall->SetActorHiddenInGame(false);
	TeleporterWall->SetActorEnableCollision(true);
	ESButton->SetActorHiddenInGame(true);
}

void ATutorialManager2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (player == nullptr)
	{
		//get player
		for (TActorIterator<ABasePlayer> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr)
			{
				player = *ActorItr;
				//disable inventory
				player->GetInventoryComponent()->SwitchToGun();
				player->GetInventoryComponent()->DisableInventory();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Disabled inventory");
			}
		}
	}
	else
	{
		//if player's health is less than the min health
		if (player->GetHealthComponent()->GetHealth() < 60.0f)
		{
			//set health to min health
			player->GetHealthComponent()->TutorialSetHealth(60.0f);
		}
	}

	if (player != nullptr && player->GetInventoryComponent()->CanAccessInventory() == true && ProgressionTracker <= 6)
	{
		player->GetInventoryComponent()->DisableInventory();
	}

	if (player != nullptr && ProgressionTracker < 6)
	{
		//constantly disable inventory if not at combat area
		player->GetInventoryComponent()->DisableInventory();
	}
	else if (ProgressionTracker == 6)
	{
		if (ACAEnemies[0]->bDied == true)
		{
			IncrementProgression();
			UnlockDoor(ShipDoors[3]);
			PlayGunCombatLine();
		}
	}
	else if (ProgressionTracker == 7)
	{
		if (GUNEnemies[0]->bDied == true &&
			GUNEnemies[1]->bDied == true)
		{
			IncrementProgression();
			UnlockDoor(ShipDoors[4]);
			LockDoor(ShipDoors[3]);
			PlayMeleeCombatLine();
		}
	}
	else if (ProgressionTracker == 8)
	{
		if (MELEEEnemies[0]->bDied == true &&
			MELEEEnemies[1]->bDied == true)
		{
			IncrementProgression();
			UnlockDoor(ShipDoors[5]);
			LockDoor(ShipDoors[4]);
			PlayGrenadeCombatLine();
		}
	}
	else if (ProgressionTracker == 9)
	{
		if (GRENADEEnemies[0]->bDied == true &&
			GRENADEEnemies[1]->bDied == true &&
			GRENADEEnemies[2]->bDied == true)
		{
			IncrementProgression();
			UnlockDoor(ShipDoors[6]);
			LockDoor(ShipDoors[5]);
			PlayHPAmmoBay();
		}
	}
	else if (ProgressionTracker == 10)
	{
		if (bAmmoDone == true && bHPDone == true)
		{
			IncrementProgression();
			UnlockDoor(ShipDoors[7]);
			LockDoor(ShipDoors[6]);
			PlayScavengerChase();
		}
	}
	else if (ProgressionTracker == 11)
	{
		//scavenger has retrieved the vaccine
		if (pScavenger != nullptr && pScavenger->bIsActive == false)
		{
			IncrementProgression();
			TeleporterWall->SetActorHiddenInGame(true);
			TeleporterWall->SetActorEnableCollision(false);
		}
	}
	else if (ProgressionTracker == 13)
	{
		if (SHIPEnemies[0]->bDied == true)
		{
			IncrementProgression();
			FActorSpawnParameters params;
			GetWorld()->SpawnActor<AVaccineObject>(vaccineSubclass, VaccineSpawnPoint->GetTransform(), params);
			PlayVaccineDropOff();
		}
	}
}

void ATutorialManager2::UnlockDoor(AShipDoor* door)
{
	if (door)
		door->FixDoor();
}

void ATutorialManager2::LockDoor(AShipDoor* door)
{
	if (door)
		door->HackedDoor();
}

//called in blueprints when you reach the first door
void ATutorialManager2::PrePlanetSelect()
{
	//tell player to select planets now
	IncrementProgression();
	PlayPS();
}

void ATutorialManager2::OnAudioFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
	switch (ProgressionTracker)
	{
	case 0: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayMovement_Reminder, 30.0f, false); break; //movement sounds
	case 1:	GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayPS_Reminder, 30.0f, false); break; //planet select reminder
	case 2: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayFixEvent_Reminder, 30.0f, false); break; //fix event reminder
	case 3: break; //just here so we don't repeat the "go through door for event description" line every time we move on the door
	case 4: UnlockDoor(ShipDoors[2]); IncrementProgression(); break; //event description or move on
	case 5: break; //just here so we don't repeat the "combat explanation" line every time we move on the door
	case 6: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayACACombatLine_Reminder, 30.0f, false); break; //Combat, ACA
	case 7: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayGunCombatLine_Reminder, 30.0f, false); break; //Combat, Gun
	case 8: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayMeleeCombatLine_Reminder, 30.0f, false); break; //Combat, Melee
	case 9: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayGrenadeCombatLine_Reminder, 30.0f, false); break; //Combat, Grenade
	case 10: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayHPAmmoBay_Reminder, 30.0f, false); break; //Ammo HP Refill
	case 11: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayScavengerChase_Reminder, 30.0f, false); break; //Go get that vaccine (here just in case the audio ends before scavenger gets across)
	case 12: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayScavengerChase_Reminder, 30.0f, false); break; //Go get that vaccine
	case 13: break; //Once player teleports, increment to 13, player will have to fight and we don't want random voice lines going off
	case 14: GetWorld()->GetTimerManager().SetTimer(TutorialSoundTimer, this, &ATutorialManager2::PlayVaccineDropOff_Reminder, 30.0f, false); break; //Go drop off that vaccine
	case 15: TutorialDone(); break; //finished tutorial
	default: break; //perish
	}
}

void ATutorialManager2::IncrementProgression()
{
	ProgressionTracker += 1;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::FromInt(ProgressionTracker));
}

//tell player to use WASD, shift, ctrl and space bar for movement
void ATutorialManager2::PlayMovement()
{
	if (SoundCues.Num() > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[0]);
		pAudioComponent->Play();
	}
}

//remind players if they're the big dumb
void ATutorialManager2::PlayMovement_Reminder()
{ 
	if (SoundCues.Num() > 1)
	{
		pAudioComponent->SetSound(SoundCues[1]); 
		pAudioComponent->Play();
	}
}

//upon reaching the door, tell the player about planet selection, fuel, maybe shop as well idk
void ATutorialManager2::PlayPS()
{
	if (SoundCues.Num() > 2)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[2]);
		pAudioComponent->Play();
	}
}

//remind the player to hit the damn planet
void ATutorialManager2::PlayPS_Reminder()
{
	if (SoundCues.Num() > 3)
	{
		pAudioComponent->SetSound(SoundCues[3]);
		pAudioComponent->Play();
	}
}

//upon hitting the planet, tell player to fix the event by hitting the button and placing the pickups in the proper spot
void ATutorialManager2::PlayFixEvent()
{
	if (SoundCues.Num() > 4)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[4]);
		pAudioComponent->Play();
	}
}

//remind the player how to fix the event
void ATutorialManager2::PlayFixEvent_Reminder()
{
	if (SoundCues.Num() > 5)
	{
		pAudioComponent->SetSound(SoundCues[5]);
		pAudioComponent->Play();
	}
}

//tell players to go to next room.  (might separate into next room cue, and event description cues but I need to set up the trigger box for that)
void ATutorialManager2::PlayEventLine()
{
	if (SoundCues.Num() > 6)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[6]);
		pAudioComponent->Play();
		UnlockDoor(ShipDoors[1]);
	}

	//Activates the events in the next room
	for (int i = 0; i < pEvents.Num(); i++)
	{
		if (pEvents.IsValidIndex(i) && pEvents[i] != nullptr)
		{
			pEvents[i]->Activate();
		}
	}
}

//tell player "these are the events, pickups, buttons, all that good stuff"
void ATutorialManager2::PlayEventDescriptionLine()
{
	if (SoundCues.Num() > 7)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		IncrementProgression();
		pAudioComponent->SetSound(SoundCues[7]);
		pAudioComponent->Play();
	}
}

//tell players about enemies and player loadout, tell player to kill them all
void ATutorialManager2::PlayACACombatLine()
{
	if (SoundCues.Num() > 8)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		player->SetCanUseSpecialAgain();
		IncrementProgression();
		pAudioComponent->SetSound(SoundCues[8]);
		pAudioComponent->Play();
	}

	//Turn off all events
	for (int i = 0; i < pEvents.Num(); i++)
	{
		if (pEvents.IsValidIndex(i) && pEvents[i] != nullptr)
		{
			pEvents[i]->bCanDeactivate = true;
			pEvents[i]->Deactivate();
		}
	}
}

//remind player about their loadout and where to use it
void ATutorialManager2::PlayACACombatLine_Reminder()
{
	if (SoundCues.Num() > 9)
	{
		pAudioComponent->SetSound(SoundCues[9]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayGunCombatLine()
{
	if (SoundCues.Num() > 10)
	{
		player->TimerEndSpecial();
		player->GetInventoryComponent()->EnableInventory();
		player->SwitchToSlotOne();
		player->SetCanAccessInventory(false);
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[10]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayGunCombatLine_Reminder()
{
	if (SoundCues.Num() > 11)
	{
		pAudioComponent->SetSound(SoundCues[11]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayMeleeCombatLine()
{
	if (SoundCues.Num() > 12)
	{
		player->GetInventoryComponent()->EnableInventory();
		player->SwitchToSlotTwo();
		player->SetCanAccessInventory(false);
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[12]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayMeleeCombatLine_Reminder()
{
	if (SoundCues.Num() > 13)
	{
		pAudioComponent->SetSound(SoundCues[13]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayGrenadeCombatLine()
{
	if (SoundCues.Num() > 14)
	{
		player->GetInventoryComponent()->EnableInventory();
		player->SwitchToSlotThree();	//not locking inventory here since you have to re-equip the grenade to detonate it
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[14]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayGrenadeCombatLine_Reminder()
{
	if (SoundCues.Num() > 15)
	{
		pAudioComponent->SetSound(SoundCues[15]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayHPAmmoBay()
{
	if (SoundCues.Num() > 16)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[16]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayHPAmmoBay_Reminder()
{
	if (SoundCues.Num() > 17)
	{
		pAudioComponent->SetSound(SoundCues[17]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayScavengerChase()
{
	if (SoundCues.Num() > 18)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[18]);
		pAudioComponent->Play();
		//TSubclassOf<AScavengerAI> subclass;
		FActorSpawnParameters params;
		pScavenger = GetWorld()->SpawnActor<AScavengerAI>(subclass, SpawnPoint->GetTransform(), params);
		if (pScavenger)
		{
			pScavenger->Activate();
		}
	}
}

void ATutorialManager2::PlayScavengerChase_Reminder()
{
	if (SoundCues.Num() > 19)
	{
		player->TimerEndSpecial(); //end special just in case it's up so the player can carry the vaccine
		pAudioComponent->SetSound(SoundCues[19]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayVaccineDropOff()
{
	if (SoundCues.Num() > 20)
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
		pAudioComponent->SetSound(SoundCues[20]);
		pAudioComponent->Play();
	}
	ESButton->SetActorHiddenInGame(false);
}

void ATutorialManager2::PlayVaccineDropOff_Reminder()
{
	if (SoundCues.Num() > 21)
	{
		pAudioComponent->SetSound(SoundCues[21]);
		pAudioComponent->Play();
	}
}

void ATutorialManager2::PlayTutorialDone()
{
	if (ProgressionTracker == 14)
	{
		if (SoundCues.Num() > 22)
		{
			GetWorld()->GetTimerManager().ClearTimer(TutorialSoundTimer);
			pAudioComponent->SetSound(SoundCues[22]);
			pAudioComponent->Play();
		}
		IncrementProgression();
	}
}

void ATutorialManager2::TutorialDone()
{
	//set tutorial to be complete and then save game
	GetGameInstance<UStellarFeverGameInstance>()->bTutorialComplete = true;
	GetGameInstance<UStellarFeverGameInstance>()->SaveGame(); 
	UGameplayStatics::OpenLevel(this, TEXT("MainMenuMap"));
}