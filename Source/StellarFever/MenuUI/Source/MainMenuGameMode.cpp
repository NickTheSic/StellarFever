// Copyright Quarantine Games 2020


#include "../MainMenuGameMode.h"
#include "../../SFBasicSaveGame.h"
#include "../../StellarFeverGameInstance.h"
#include "../MainMenu.h"
#include "../MainMenuHUD.h"
#include "../MainMenuSwitcher.h"
#include "../MapSelectMenu.h"
#include "Kismet/GameplayStatics.h"
#include "../OptionsMenu.h"
#include "Components/Slider.h"

void AMainMenuGameMode::BeginPlay()
{
	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	//Set the bool in the game instance to true
	if (sfInstance)
		sfInstance->LoadGame();

}

TArray<FName> AMainMenuGameMode::GetAllMaps()
{
	//This can crash if the player controller is null on a client, 
	//where the menu and begin play happens before the client loads in perfectly
	//if (UGameplayStatics::GetPlayerController(this, 0) == nullptr);

	TArray<FString> Mapfiles;
	TArray<FName> MapNames;


	 //A function I found online for getting all the map file paths and converting it to just a name
	IFileManager::Get().FindFilesRecursive(Mapfiles, *FPaths::ProjectContentDir(), TEXT("*.umap"), true, false, false);

	//Remove the rest of the path so we are left with the Name
	for (int32 i = 0; i < Mapfiles.Num(); i++)
	{

		int32 lastSlash = -1;
		if (Mapfiles[i].FindLastChar('/', lastSlash))
		{

			FString mapName;

			//Start at the last slash and get all characters up to the .umap
			for (int32 j = lastSlash + 1; j < Mapfiles[i].Len() - 5; j++)
			{
				mapName.AppendChar(Mapfiles[i][j]);
			}

			MapNames.Add(FName(mapName));
		}
	}
	
	return MapNames;
}

void AMainMenuGameMode::SetLoadMap(FName map)
{
	MapToLoad = map;
}