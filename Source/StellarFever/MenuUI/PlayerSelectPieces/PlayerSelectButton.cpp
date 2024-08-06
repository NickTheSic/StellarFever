// Copyright Quarantine Games 2020

#include "PlayerSelectButton.h"
#include "../PlayerSelectMenuBase.h"
#include "PlayerInformationDisplayBox.h"

UPlayerSelectButton::UPlayerSelectButton()
{
	//Setup the Delegate
	OnClicked.AddDynamic(this, &UPlayerSelectButton::SetPlayerOnClicked);
	OnHovered.AddDynamic(this, &UPlayerSelectButton::SetPlayerInformation);
}

void UPlayerSelectButton::SetPlayerOnClicked()
{
	pPlayerSelectMenu->SetPlayerClass(ButtonClass);

	SetPlayerInformation();

	//When we select a player we want to let controllers just instantly hit x again to select that player
	if (pPlayerSelectMenu->IsValidLowLevel())
	{
		pPlayerSelectMenu->SelectButton->SetFocus();
	}

}

void UPlayerSelectButton::SetPlayerInformation()
{
	if(!HasKeyboardFocus())
		SetFocus();

	//Set the information
	if (pPlayerInformationBox->IsValidLowLevel())
	{
		pPlayerInformationBox->SetPlayerClass(ButtonClass);
		pPlayerInformationBox->SetPlayerName(CharacterName);
		pPlayerInformationBox->SetWeaponInformationText(WeaponInformation);
		pPlayerInformationBox->SetSpecialInformationText(SpecialInformation);
		pPlayerInformationBox->SetPlayerFlavourText(FlavourText);
	}
}
