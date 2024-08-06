// Copyright Quarantine Games 2020


#include "StellarFeverSinglePlayerGameMode.h"
#include "../StellarFeverGameInstance.h"
#include "../Player/BasePlayer.h"
#include "../Player/CaptainPlayer.h"
#include "../Player/EngineerPlayer.h"
#include "../Player/GunnerPlayer.h"
#include "../Player/MedicPlayer.h"


void AStellarFeverSinglePlayerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	if (UStellarFeverGameInstance* instance = GetGameInstance<UStellarFeverGameInstance>())
	{
		switch (instance->PlayerOneSelectedClass)
		{
		case EPlayerClass::Captain:
			DefaultPawnClass = CaptainPlayerTemplate;
			break;
		case EPlayerClass::Engineer:
			DefaultPawnClass = EngineerPlayerTemplate;
			break;
		case EPlayerClass::Gunner:
			DefaultPawnClass = GunnerPlayerTemplate;
			break;
		case EPlayerClass::Medic:
			DefaultPawnClass = MedicPlayerTemplate;
			break;
		default:
			DefaultPawnClass = ABasePlayer::StaticClass();
			UE_LOG(LogTemp, Error, TEXT("There was no selected class inside the game instance, spawning ABasePlayer::StaticClass()"));
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The Instance was null so we are spawning ACaptainPlayer"));
		DefaultPawnClass = CaptainPlayerTemplate;
	}

	Super::InitGame(MapName, Options, ErrorMessage);
}

void AStellarFeverSinglePlayerGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (TrashBagTemplate != nullptr && TrashBagSpawnLocations.Num() > 0)
	{
		for (FVector loc : TrashBagSpawnLocations)
		{
			GetWorld()->SpawnActor<AActor>(TrashBagTemplate, FTransform(loc));
		}
	}
}
