// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "JoinMenu.generated.h"

/**
 * 
 */


USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};


UCLASS()
class STELLARFEVER_API UJoinMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* BackButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* JoinButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* RefreshButton = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UScrollBox* ServerList = nullptr;


	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;

	UPROPERTY(EditAnywhere, Category = "")
		TSubclassOf<class UServerInfoRow> ServerRowTemplate = nullptr;


public:

	virtual void NativeOnInitialized() override;

	void SetServerList(TArray<FServerData> ServerNames);

	UFUNCTION()
		void OnClickBack();

	UFUNCTION()
		void JoinServer();

	UFUNCTION()
		void RefreshServers();

	void SelectIndex(uint32 index);

private:

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();

};
