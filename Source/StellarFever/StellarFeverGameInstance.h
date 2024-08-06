// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Player/PlayerInformation.h"
#include "SFBasicSaveGame.h"

#include "Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"

//#include "OnlineBlueprintCallProxyBase.h"
#include "OnlineSubsystemUtils.h"

#include "StellarFeverGameInstance.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FFriendData
{
	GENERATED_USTRUCT_BODY()

/// I took this from Tilans game instance for now

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString RealName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString Presence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString UniquiNetID;

};


UCLASS()
class STELLARFEVER_API UStellarFeverGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UFUNCTION()
		void SetGlobalMix();

public:
	
	UStellarFeverGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;
	
	UFUNCTION(Exec)
		void Host(FString ServerName, FString Map);
	UFUNCTION(Exec)
		void Join(uint32 Index);


	UFUNCTION(Exec)
		void ForceCrashTheGame() {
			checkf(false, TEXT("Forced a crash to game."));
		}

	UPROPERTY(BlueprintReadWrite)
		class USoundMix* pGlobalMix = nullptr;
	UPROPERTY(BlueprintReadWrite)
		class USoundClass* pGeneralSoundClass = nullptr;
	UPROPERTY(BlueprintReadWrite)
		class USoundClass* pMusicSoundClass = nullptr;
	UPROPERTY(BlueprintReadWrite)
		class USoundClass* pVoiceSoundClass = nullptr;

	UFUNCTION()
		void UpdateVolume(float general, float music, float voice);

	void StartSession();
	void RefreshServerList();

	void RetrieveFriendsList();
	TArray<FFriendData>FriendsArr;
	FOnReadFriendsListComplete  ReadCompleteDelegate;

	void OnlineLoadLevel(FString mapname);

private:
	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result);


public:

	void SetIsOnlineGame(bool bNewIsOnline) { bIsOnlineGame = bNewIsOnline; }
	bool bIsOnlineGame = false;

	bool bPlayerWasChosen = false; //A check to see if we selected a player
	EPlayerClass PlayerOneSelectedClass; //The class the player selected

	TMap<FUniqueNetIdRepl, EPlayerClass> OnlineSelectedPlayerMap;

	void AddClassToPlayerMap(APlayerState* InPlayerState, const EPlayerClass& InPlayerClass);


	UPROPERTY()
		bool bTutorialComplete = false;
	UPROPERTY()
		float fBrightness = 50.0f;
	UPROPERTY()
		float fLookSensitivity = 10.0f;
	UPROPERTY()
		float fGeneralVolume = 25.0f;
	UPROPERTY()
		float fMusicVolume = 25.0f;
	UPROPERTY()
		float fVoiceVolume = 25.0f;
	UPROPERTY()
		EInvertControllerAxis eInvertAxis = EInvertControllerAxis::Invert_None;

	
	class UJoinMenu* pJoinMenu = nullptr;

	IOnlineSessionPtr SessionInterface;


	UPROPERTY()
	class USFBasicSaveGame* pSaveData = nullptr;
	void SaveGame();
	void LoadGame();
	void CreateSave();


private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AStellarFeverGameModeBase> m_OnlineGameMode;

	FString DesiredServerName;

	FOnlineSessionSettings SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;



	/** Loading Screen */

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UUserWidget> LoadingScreenWidget;

	UPROPERTY()
		class UUserWidget* pLoadingScreenWidget;

public:

	//Thank you https://ue4community.wiki/legacy/loading-screen-243mzpq1
	UFUNCTION()
		void BeginLoadingScreen(const FString& InMapName);
	UFUNCTION()
		void EndLoadingScreen(UWorld* InLoadedWorld);

};
