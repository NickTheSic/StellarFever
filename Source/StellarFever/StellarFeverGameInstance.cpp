// Copyright Quarantine Games 2020

#include "StellarFeverGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemNames.h"
#include "Interfaces/OnlineSharingInterface.h"

#include "MenuUI/JoinMenu.h"

#include "StellarFeverGameModeBase.h"
#include "SFBasicSaveGame.h"

#include "Sound/SoundClass.h"

#include "MoviePlayer.h"

static const FName SESSION_NAME = TEXT("StellarFever");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("StellarFeverServer");


UStellarFeverGameInstance::UStellarFeverGameInstance(const FObjectInitializer& ObjectInitializer)
{

}

void UStellarFeverGameInstance::SetGlobalMix()
{
	if (pGlobalMix != nullptr)
		UGameplayStatics::SetBaseSoundMix(GetWorld(), pGlobalMix);
}

void UStellarFeverGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UStellarFeverGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UStellarFeverGameInstance::EndLoadingScreen);
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UStellarFeverGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UStellarFeverGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UStellarFeverGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UStellarFeverGameInstance::OnJoinSessionComplete);

		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameInstance Init() - Session Interface wasn't valid"));
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Init() - No subsystem found"));
	}
	pGlobalMix = LoadObject<USoundMix>(this, TEXT("/Game/AudioAssets/AudioClass/GlobalSoundMix"));
	pGeneralSoundClass = LoadObject<USoundClass>(this, TEXT("/Game/AudioAssets/AudioClass/GeneralSoundClass"));
	pVoiceSoundClass = LoadObject<USoundClass>(this, TEXT("/Game/AudioAssets/AudioClass/VoiceSoundClass"));
	pMusicSoundClass = LoadObject<USoundClass>(this, TEXT("/Game/AudioAssets/AudioClass/MusicSoundClass"));


	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UStellarFeverGameInstance::SetGlobalMix);
}

void UStellarFeverGameInstance::Host(FString ServerName, FString Map)
{
	DesiredServerName = ServerName;
	
	RetrieveFriendsList();

	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession !=nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UStellarFeverGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

}

void UStellarFeverGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

void UStellarFeverGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		if (SessionSettings.bIsLANMatch)
			SessionSearch->bIsLanQuery = true;
		else
			SessionSearch->bIsLanQuery = false;


		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UStellarFeverGameInstance::RetrieveFriendsList()
{
	auto Friends = Online::GetFriendsInterface();
	if ( Friends.IsValid() )
	{
		ULocalPlayer* Player = Cast<ULocalPlayer>(GetWorld()->GetFirstLocalPlayerFromController());
		Friends->ReadFriendsList(Player->GetControllerId(), EFriendsLists::ToString(EFriendsLists::Default), ReadCompleteDelegate);
	}
}

void UStellarFeverGameInstance::OnlineLoadLevel(FString mapname)
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;


	FString GameMap = "/Game/Maps/";
	GameMap.Append(mapname);
	GameMap.Append("?listen");

	GameMap.Append("?Game=");

	if (m_OnlineGameMode)
		GameMap.Append(m_OnlineGameMode->GetPathName());
	else
		GameMap.Append("/Game/Blueprints/BP_MultiplayerGameMode.BP_MultiplayerGameMode_C");


	if (SessionSettings.bIsLANMatch)
	{
		GameMap.Append("?bIsLanMatch=1");
	}

	//Set it so new players can't join the game once it starts
	SessionSettings.bShouldAdvertise = false;
	SessionSettings.bUsesPresence = false;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.bAllowJoinViaPresence = false;
	SessionInterface->UpdateSession(SESSION_NAME, SessionSettings);

	World->ServerTravel(GameMap);
}

void UStellarFeverGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FName name = IOnlineSubsystem::Get()->GetSubsystemName();
		if (name.IsEqual("NULL"))
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Interface wasn't valid"));
	}
}


void UStellarFeverGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance:OnCreateSessionComplete:172 - could not create session"));
		return;
	}

	//Tilan tears down his menu here

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	
	FString PlayerSelectMap = "/Game/Maps/PlayerSelect?listen";
	if (SessionSettings.bIsLANMatch)
	{
		PlayerSelectMap.Append("?bIsLanMatch=1");
	}

	World->ServerTravel(PlayerSelectMap);
}


void UStellarFeverGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}


void UStellarFeverGameInstance::OnFindSessionsComplete(bool Success)
{
	auto Results = SessionSearch->SearchResults;
	if (Success && SessionSearch.IsValid() && pJoinMenu->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Log, TEXT("Finished Find Session"));

		TArray<FServerData> ServerNames;

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;

			Data.Name = SearchResult.GetSessionIdStr();
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUsername = SearchResult.Session.OwningUserName;

			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;
			}
			else
			{
				Data.Name = "Could not find name.";
			}

			ServerNames.Add(Data);
		}
		

		pJoinMenu->SetServerList(ServerNames);
		
	}
}


void UStellarFeverGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;


	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance JoinSessionComplete 253 - Could not get connect string."));
		return;
	}


	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), &Address));


	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

}


void UStellarFeverGameInstance::AddClassToPlayerMap(APlayerState* InPlayerState, const EPlayerClass& InPlayerClass)
{
	if (!ensure(InPlayerState != nullptr)) return;

	FUniqueNetIdRepl id = InPlayerState->GetUniqueId();

	if (OnlineSelectedPlayerMap.Contains(id))
		OnlineSelectedPlayerMap[id] = InPlayerClass;
	else
		OnlineSelectedPlayerMap.Add(id, InPlayerClass);
}


void UStellarFeverGameInstance::SaveGame()
{
	if (!pSaveData)
		LoadGame();
	pSaveData->fBrightness = fBrightness;
	pSaveData->bTutorialComplete = bTutorialComplete;
	pSaveData->fLookSensitivity = fLookSensitivity;
	pSaveData->fGeneralVolume = fGeneralVolume;
	pSaveData->fVoiceVolume = fVoiceVolume;
	pSaveData->fMusicVolume = fMusicVolume;
	pSaveData->eInvertAxis = eInvertAxis;
	//pSaveData->fVolume = fVolume;
	UGameplayStatics::SaveGameToSlot(pSaveData, "StellarFever_Basic", 0);
}

void UStellarFeverGameInstance::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist("StellarFever_Basic", 0))
		pSaveData = Cast<USFBasicSaveGame>(UGameplayStatics::LoadGameFromSlot("StellarFever_Basic", 0));
	else
		CreateSave();

	fBrightness = pSaveData->fBrightness;
	bTutorialComplete = pSaveData->bTutorialComplete;
	fLookSensitivity = pSaveData->fLookSensitivity;
	eInvertAxis = pSaveData->eInvertAxis;
	UpdateVolume(pSaveData->fGeneralVolume, pSaveData->fMusicVolume, pSaveData->fVoiceVolume);
}

void UStellarFeverGameInstance::CreateSave()
{
	//Default save data
	pSaveData = Cast<USFBasicSaveGame>(UGameplayStatics::CreateSaveGameObject(USFBasicSaveGame::StaticClass()));
	pSaveData->fBrightness = 50.f;
	pSaveData->fLookSensitivity = 8.f;
	pSaveData->fGeneralVolume = 50.f;
	pSaveData->fVoiceVolume = 50.f;
	pSaveData->fMusicVolume = 50.f;
	pSaveData->bTutorialComplete = false;
}

void UStellarFeverGameInstance::UpdateVolume(float general, float music, float voice)
{
	fGeneralVolume = FMath::Clamp(general, 1.f, 100.f);
	fMusicVolume = FMath::Clamp(music, 1.f, 100.f);
	fVoiceVolume = FMath::Clamp(voice, 1.f, 100.f);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), pGlobalMix, pGeneralSoundClass, fGeneralVolume / 100.f, 1.f, 0.f);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), pGlobalMix, pMusicSoundClass, fMusicVolume / 100.f, 1.f, 0.f);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), pGlobalMix, pVoiceSoundClass, fVoiceVolume / 100.f,1.f, 0.f);
}


void UStellarFeverGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		pLoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidget);
		LoadingScreen.WidgetLoadingScreen = pLoadingScreenWidget->TakeWidget();
		//LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
		

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UStellarFeverGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{

}