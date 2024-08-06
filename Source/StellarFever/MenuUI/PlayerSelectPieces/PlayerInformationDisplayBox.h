// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Player/PlayerInformation.h"
#include "PlayerInformationDisplayBox.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPlayerInformationDisplayBox : public UUserWidget
{
	GENERATED_BODY()
	

public:

/******  These are all the UI Elements I want to have to display player info  *****/
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ClassNameText = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CharacterNameText = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponInformationText = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SpecialInformationText = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* FlavourText = nullptr;

public:

	void NativeOnInitialized() override; //Used as BeginPlay

	void SetPlayerClass(const EPlayerClass& playerClass);
	void SetPlayerName(const FText& playerName);
	void SetPlayerFlavourText(const FText& flavourText);
	void SetWeaponInformationText(const FText& weaponInfoText);
	void SetSpecialInformationText(const FText& specialInfoText);

};
