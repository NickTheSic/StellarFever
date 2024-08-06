// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "../../Player/PlayerInformation.h"
#include "../MiscMenuPieces/SFButton.h"
#include "PlayerSelectButton.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UPlayerSelectButton : public USFButton
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
		EPlayerClass ButtonClass;

	UPROPERTY(EditAnywhere)
		FText CharacterName = FText::FromString(TEXT("Player Long-Name"));

	UPROPERTY(EditAnywhere)
		FText FlavourText = FText::FromString( TEXT("I am super duper backstroy text \nAlong with Other flavour text ") );
	UPROPERTY(EditAnywhere)
		FText WeaponInformation = FText::FromString(TEXT("This is your gun, it normal fires and alt fires."));
	UPROPERTY(EditAnywhere)
		FText SpecialInformation = FText::FromString(TEXT("Press Q and you can Heal, Teleport and even stun enemies"));


	//The corresponding information box to display the info in
	class UPlayerInformationDisplayBox* pPlayerInformationBox = nullptr;

	//The owning player select menu
	class UPlayerSelectMenuBase* pPlayerSelectMenu = nullptr;

public:
	UPlayerSelectButton();

	UFUNCTION()	//This is bound to the OnClicked delegate
		void SetPlayerOnClicked();

	UFUNCTION()
		void SetPlayerInformation();

};
